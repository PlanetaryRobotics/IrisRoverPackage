"""
Wrapper for interfacing with MongoDB, including accessing collections in the
database and accessing all changed documents.

This should be the only place the BIL is aware that we're using MongoDB and not
some other document-based database. 

@author: Connor W. Colombo (CMU)
@last-updated: 01/07/2023
"""
# Activate postponed annotations (for using classes as return type in their own methods):
from __future__ import annotations

from typing import Final, Optional, Iterable,
from typing_extensions import TypeAlias

import pymongo
from gridfs import GridFS
import enum

import certifi

# Type Aliases so the outer code doesn't need to know we're using MongoDB:
Client: TypeAlias = pymongo.MongoClient
Database: TypeAlias = pymongo.database.Database
Collection: TypeAlias = pymongo.collection.Collection
Cursor: TypeAlias = pymongo.cursor.Cursor
FileSystem: TypeAlias = GridFS


def connect_client(
    server: str,
    user: str,
    code: str,
    protocol: str = 'mongodb+srv'
) -> Client:
    ca = certifi.where()
    url = f"{protocol}://{user}:{code}@{server}/?retryWrites=true&w=majority"
    return pymongo.MongoClient(url, tlsCAFile=ca)


class State(enum.Enum):
    NOT_SENT = "NOT_SENT"
    SUCC_SENT = "SUCC_SENT"
    SUCC_EXEC = "SUCC_EXEC"
    FAIL = "FAIL"


class CollectionName(enum.Enum):
    """Enumeration of all valid collection names."""
    COMMANDS = 'Commands'
    IMAGES = 'Images'


class CollectionConnection(Iterable):
    client: Client
    db: Database
    collection: Collection
    cursor: Optional[Cursor]
    _fs: Optional[FileSystem]
    _changes: Optional[_ChangeIterator]

    # Selector indicating which fields of a document in a collection are intended for the backend:
    BACKEND_FIELD_SELECTOR = {
        'Commands': {'lookupID': 1, 'name': 1, 'args': 1},
        'Images': {'lookupID': 1, 'commandLookupID': 1, 'bin': 1, 'camera': 1, 'sendTime': 1}
    }

    # Iteration wrapper for pymongo's collection cursor.
    # Allows for
    class _ChangeIterator:
        """ Iterable wrapper for pymongo's iterable collection cursor which
        extracts the payload intended for the backend from a command document.
        This exists within its own class instead of __iter__ and __next__
        methods of the outer Collection class since the iterator in the outer
        class is used to iterate through all documents in the collection.
        """

        def __init__(self, collection):
            self.collection = collection
            # Watch for insert, replace, and update operations - not delete:
            self.cursor = collection.watch(full_document='updateLookup', pipeline=[
                {'$match': {'operationType': {'$ne': 'delete'}}},
                {'$match': {'operationType': {'$ne': 'update'}}},
                {'$project': {
                    'fullDocument': self.BACKEND_FIELD_SELECTOR[self.collection.name]}}
            ])

        def __iter__(self):
            return self

        def __next__(self):
            return next(self.cursor)['fullDocument']

        def try_next(self):
            change = self.cursor.try_next()
            return change['fullDocument'] if change is not None else None

        def get_all_new(self):
            all_new = []

            change = self.cursor.try_next()
            while change is not None:
                all_new.append(change['fullDocument'])
                change = self.cursor.try_next()

            return all_new

    def __init__(self, client: Client, partition: str, collection: CollectionName):
        assert isinstance(client, Client)
        assert isinstance(collection, CollectionName)
        self.db = self.client[partition]
        self.collection = self.db[collection.value]
        self.cursor = None  # cursor is only init'd when __iter__ starts

    @property
    def changes(self) -> _ChangeIterator:
        # Only init the change iterator if it's needed:
        if self._changes is None:
            self._changes = self._ChangeIterator(self.collection)
        return self._changes

    @property
    def fs(self) -> FileSystem:
        # Only init the FS if it's needed:
        if self._fs is None:
            self._fs = GridFS(self.db)
        return self._fs

    def __iter__(self):
        """ Get iterator for the backend payloads of each document in the
            collection.
            NOTE: This iteration won't necessarily be in any particular or
            consistent order.
        """
        self.cursor = self.collection.find(
            {},
            self.BACKEND_FIELD_SELECTOR[self.collection._Collection__name]
        )
        return self.cursor

    def __next__(self):
        """Iterate through all documents in the collection"""
        return next(self.cursor)

    def count(self):
        """ Get an accurate count across entire collection across DB, even if
            collection is sharded or documents are orphaned.
        """
        counter = self.collection.aggregate([
            {'$group': {'_id': None, 'numDocs': {'$sum': 1}}},
            {'$project': {'_id': 0}}
        ])
        # Extract result document from aggregation cursor.
        counter = [x for x in counter]
        # Extract count from document:
        return counter[0]['numDocs'] if len(counter) > 0 else 0

    def write(self, doc):
        """ Adds the given document to the DB collection. Sets the lookupID to
            the increment of the accurate count of all documents across the
            database.
        """
        doc['lookupID'] = self.count() + 1
        self.collection.insert_one(doc)

    def fetch(self, lid, include_everything=False):
        """ Returns the contents of the backend payload of the document which
            has the given lookupID, `lid`.
            lookupID can be a MongoDB query selector.
            Alternatively, `includeEverything` can be flagged if more data
            (not just data earmarked for the backend) is desired.
        """
        selector = {
            '_id': 0} if include_everything else MongoDbCollection.BACKEND_FIELD_SELECTOR[self.collection._Collection__name]
        if isinstance(lid, dict):  # if query selector given
            csr = self.collection.find({'lookupID': lid}, selector)
            return [c for c in csr]  # return as a list (expand the cursor)
        else:
            return self.collection.find_one({'lookupID': lid}, selector)

    def update(self, lid, doc):
        """ Updates the contents of the document with the given lookupID, `lid`,
        overwriting any fields which are given in the given doc.
        lookupID can be a MongoDB query selector.
        """
        if isinstance(lid, dict):  # if query selector given
            self.collection.update_many(
                {'lookupID': lid}, {'$set': doc}, upsert=False)
        else:
            self.collection.update_one(
                {'lookupID': lid}, {'$set': doc}, upsert=False)
