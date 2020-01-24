'use strict';
/*
 * Set of All Functionality for Pushing or Pulling JSON Data to/from the Database.

 * Author: Sofia Hurtado, CMU
 * Last Update: 11/8/2019, Colombo
 */
 // TODO: Let user w/o permissions push command to db but have it flagged and ignored. (this behavior should be moved to CommandField.vue)
 // TODO: Update connected & currentlyConnected everywhere it could be caught (#onCollection, #onClient, etc.)

import mongo from 'mongodb'
const streamConcat = require('concat-stream')

const { EventEmitter } = require('events'); // Different than Vue eventHub (since DBInterface exists outside of Vue renderer)
/* Events List:
  - statusChange: DB connection status has changed. Receives one argument which is an object containing:
    {
      connected: // new connection status.
    }
*/


import Collections from './Collections'
import DBObject from '@/data_classes/DBObject.js'


// --- PRIVATE: ---

// All Acceptable Mission IDs: (note, all mission names are forced to be lowercase through #init)
const Missions = [
  "hermes",
  "paper", // NOTE: Login.vue allows there to be any number of Missions Named "PaperXXX..."
  "dev"
];

/* Database Addresses for Each Mission (w/out passcodes) as lambdas which fill
out a template literal from given passcode and partition as inputs.
*/
const DB_URLs = {
  hermes: (code,user) => `mongodb://CubeRoverAdmin:${code}@127.0.0.1:27017/${user}?retryWrites=true`,
  default: (code,user) => `mongodb+srv://CubeRoverAdmin:${code}@devcluster-3thor.mongodb.net/${user}?retryWrites=true`
}

// Primary Databases Users for Each Known Mission:
const DB_Users = {
  hermes: "admin",
  dev: "test",
  default: "test"
}

// Database Addresses for Each Known Mission: (note: acceptable partition names must be lowercase (by internal convention, not technical requirement))
const DB_Partitions = {
  hermes: "hermes",
  paper: "paper",
  dev: "test"
}

let mission = "";
let passcode = "";

function DB_URL(){
  let selector;
  if(mission in DB_URLs){
    selector = mission;
  } else{
    selector = "default";
  }
  return DB_URLs[selector](passcode, DB_USER());
}

function DB_USER(){
  let selector;
  if(mission in DB_URLs){
    selector = mission;
  } else{
    selector = "default";
  }
  return DB_Users[selector];
}

function DB_PARTITION(){
  if(mission in DB_Partitions){
    return DB_Partitions[mission];
  } else{
    return mission;
  }
}

let connected = false; // whether the DB has ever been init'd and successfully connected
let currentlyConnected = false; // Whether the DB is currentlyConnected (not externally accessible, updated thru #checkConnection)

// --- PUBLIC: ---

export default {
  eventBus: new EventEmitter(),

  // Returns whether a connection has even been successfully established with
  // the DB during the lifetime of the program (not necessarily whether it is
  // currently connected).
  connectionEstablished: function(){
    return connected;
  },

  get missionIDs(){
    return Missions;
  },

  /* Attempts to Connect to and Setup the Database for the Mission with the
  MissionID which uses the Given Passcode.
  Returns true if successfully connected and formatted. */
  init: async function(missionID, code){
    mission = missionID.toLowerCase();
    passcode = code;

    let connected = await this.checkConnection();
    this.verifyCollectionFormatting(); // Do this only once, on startup

    return connected;
  },

  // Performs a Synchronous Check to See if a Connection with the Database Can
  // be Established:
  checkConnection: async function(){
    let MongoClient = mongo.MongoClient;
    return await new Promise( (resolve) => {
      MongoClient.connect(DB_URL(), {useNewUrlParser: true}, (err,client) => {
        let conn;
        if(err){
          console.warn(err);
          conn = false;
        } else{
          conn = client && client.topology && client.topology.isConnected();
        }

        connected |= conn; // update whether the DB has ever been initialized and connected
        if(currentlyConnected != conn){ // if connection status has changed
          currentlyConnected = conn;
          this.eventBus.emit('statusChange', {connected: currentlyConnected});
        }
        resolve(currentlyConnected);
        client.close();
      });
    });
  },

  // Helper function that returns the MongoDB query object that finds the given
  // lookupID
  lid2Query: function(lid){
    return lid ? { lookupID: lid } : {}; // ensure that the lookupID > 0 and pass {} through if given.
  },

  // Ensures Proper Formatting of All Collection Data. Designed to be Called
  // Once on Startup. Likely won't need to be called by any applications once
  // final application is deployed. Shouldn't hurt, though.
  verifyCollectionFormatting: function(){
    Object.values(Collections.enumerators).forEach( c => { // For each valid collection:
      if(c){
        this.onCollection(c, ({collection, client}) => {
          // Ensure there is an Index Called 'lookupID'
          collection.createIndex({lookupID: 1}, err => {
            if(err){
              throw err;
            }
          });
          client.close();
        })
      }
    });
  },

  // Returns whether the the Given Collection is Valid. If not, it logs an error.
  validCollection: function(c){
    let valid = Collections.isValid(c);
    if(!valid){
      console.error(c + " is not a valid Database Collection.");
    }
    return valid;
  },

  // Checks if the Given User Has Permission to Send Commands
  checkSendPermission: function(userName){
    console.log(`User ${userName} has universal permissions for now.`);
    return true; // TODO: Do real permissions check
  },

  /*
    Attempts to Connect to the DB_URL. Invokes the Callback #command and Supplies
    as an Argument the Connected MongoDB Client.
    ***NOTE:  It is the caller's responsibility to close the client when done
              using it (done with current operations involving the db) by
              calling #client.close().
  */
  onClient: async function(command){
    let MongoClient = mongo.MongoClient;
    return new Promise( (resolve,reject) => {
      MongoClient.connect(DB_URL(), {useNewUrlParser: true}, (err,client) => {
        err ? reject(err) : resolve(command(client));
      });
    });
  },

  /*
    Attempts to Create a MongoClient Connected to DB_URL, Access DB_PARTITION,
    and Grab the Requested Collection. If the Collection doesn't exist on the db,
    it is created if you try adding something to it.
    Invokes the Callback #command and Supplies, as an Argument, an object
    containing the Database Collection Object, and the Connected MongoDB Client.
    Logs error if collection isn't a valid collection (in the Collections Enum).
    Collection can be given as either a string or a Collection Enum Symbol.
    ***NOTE:  It is the caller's responsibility to close the client when done
              using it (done with current operations involving the db) by
              calling #client.close().
    ***
  */
  onCollection: async function(c, command){
    return new Promise( (resolve,reject) => {
      if(this.validCollection(c)){ // Verify if given collection is valid:
        let commandComplete = this.onClient(client => {
          let collection = client.db(DB_PARTITION()).collection(Collections.toString(c));
          return command({client, collection});
        });
        commandComplete.then( success => resolve(success), fail => reject(fail) );
      } else{
        reject(true);
      }
    });
  },

  /*
    Writes the Given Object to the Given Database Collection (can be given as
    either a string or a Collection Enum Symbol).
    Returns a Promise that resolves with the lookupID of the document written.
  */
  write: async function(c, obj){
     return new Promise( (resolve,reject) => {
       let connected = this.onCollection(c, ({client, collection}) => {
         // Determine Total Number of Elements Currently in Collection:
         const counted = new Promise( (res) => {
           // use #aggregate in case collection is sharded or documents are orphaned
           collection.aggregate([
             { $group: { _id: null, numDocs: { $sum: 1 } } },
             { $project: { _id: 0 } }
           ]).toArray(function(err, result){
             if(err){
               reject(err);
               throw err;
             }
             res(result[0] ? result[0].numDocs : 0); // accounts for case of empty collection
           });
         });

         counted.then(count => {
           let data;
           if(obj instanceof DBObject){
             data = obj.plainJSON();
           } else{
             console.error(`Attempting to write non-DBObject to database: ${JSON.stringify(obj)}`);
             data = Object.assign({}, obj); // make a copy.
           }
           Object.assign(data, {lookupID: count+1}); // Append or modify lookupID of obj

           collection.insertOne(data, function(err){
             if(err){
               console.error(`Failed to Post ${JSON.stringify(obj)} to ${collection}.`);
               reject(err);
               throw err;
             }

             resolve(count+1); // Return the lookupID of the element written.
             client.close();
           });
         });
         return true; // For return from #onCollection
       });
       connected.then(
         ()=>{},
         err => reject(err)
       );
     });
   },

   /* Reads the 'n' Most Recent Elements from the Given Database Collection.
     Collection can be given as either a string or a Collection Enum Symbol.
     Returns a Promise that resolves with the requested docs.
   */
   getLast: async function(collection, n){
      return new Promise( (resolve,reject) => {
        let connected = this.onCollection(collection, ({client, collection}) => {
          collection.find().sort({_id: -1}).limit(n).toArray(function(err, docs){
            if(err){
              reject(err);
              throw err;
            }
            resolve(docs);

            client.close();
          });
          return true; // For return from #onCollection
        });
        connected.then(
          ()=>{},
          err => reject(err)
        );
      });
   },

  /*
    Reads the Object at the Given Lookup ID in the Given Database Collection.
    Collection can be given as either a string or a Collection Enum Symbol.
    If lookupID is false (as in: "I don't care about the lookupID"), all entries
    in the collection are returned.
    Returns a Promise that resolves with the requested docs.
  */
  read: async function(collection, lid){
    return new Promise( (resolve,reject) => {
       let connected = this.onCollection(collection, ({client, collection}) => {
         let query = this.lid2Query(lid);
         collection.find(query).toArray(function(err, docs){
           if(err){
             reject(err);
             throw err;
           }
           resolve(docs);

           client.close();
         });
         return true; // For return from onCollection
       });
       connected.then(
         ()=>{},
         err => reject(err)
       );
     });
   },

   /*
     Reads the File with the Given ID from GridFS.
     Returns a Promise that resolves with the data from the file as a full
     buffer of bytes.
   */
   readFullFile: async function(id){
     return await new Promise( (resolve,reject) => {
       let operationComplete = this.onClient(client => {
         let db = client.db(DB_PARTITION());
         let bucket = new mongo.GridFSBucket(db);
         let stream = bucket.openDownloadStream(id);

         stream.pipe(streamConcat( buf => {
           resolve(buf);
           client.close(); // only close after done using stream
         }));
         stream.on('error', err => {
           reject(err);
           client.close();
         });

         return true; // For return from onCollection
       });
       operationComplete.then(
         ()=>{},
         err => reject(err)
       );
      });
    },

   /*
    Updates the Document with the lookupID Given by 'lid' to 'newVal'.
    If the document at 'lid' has more elements than given in 'newVal', only
    those given in 'newVal' are updated / added. Extra entries in the document
    being updated are not removed.
    'lid' can be a MongoDB query object. Ex: 'lid = {$gt: 5, $lt: 12}' updates
    all documents with a lookupID > 5 and less than 12.

    Returns a Promise which resolves with success / failure as a boolean.
    */
   update: async function(collection, lid, newVal){
     return new Promise( (resolve,reject) => {
       let connected = this.onCollection(collection, ({client, collection}) => {
         let query = this.lid2Query(lid);
         let result = (err) => {
           if(err){
             reject(err);
             throw err;
           }
           resolve(!err);
           client.close();
         };
         if(typeof lid == 'number'){ // Single lookupID requested
           collection.updateOne(query, {$set: newVal}, {upsert: false}, result);
         } else{ // 'lid' could be selecting multiple documents
           collection.updateMany(query, {$set: newVal}, {upsert: false}, result);
         }
       });
       connected.then(
         ()=>{},
         err => reject(err)
       );
     });
   },

   /* Uses a ChangeStream to call the given callback whenever there is a change
      to the data in the specified collection.
      Callback receives an argument containing metadata about the change. If
      you want to access the changed document, use change.fullDocument. The
      change type is given under change.operationType can be 'insert', 'update',
      'delete'.
      Returns a promise which resolves with whether the collection was
      connected to successfully.
   */
   onChange(collection, callback, streamIdx){
     if(streamIdx === undefined){ // if not given a streamIdx, create a new one
       streamIdx = this.changeStreamLog.length;
       this.changeStreamLog[streamIdx] = null;
     }
     const pipeline = [ // Aggregation pipeline
     {
       $project: { documentKey: false }
     }
     ];
     const options = {
       fullDocument: 'updateLookup', // return fullDocument from update
       resumeAfter: this.changeStreamLog[streamIdx]
     };
     return new Promise( (resolve,reject) => {
        let connected = this.onCollection(collection, ({collection}) => {
          collection.watch(pipeline, options)
            .on("change", (change) => {
              this.changeStreamLog[streamIdx] = change._id;
              callback(change);
            })
            .on("error", err => {
              // Change Stream connection times out if nothing new has come in in a while; so, re-watch stream
              console.log("Change Stream Error for " + collection.collectionName + ": " + err);
              console.log("Attempting to establish ChangeStream . . .");
              this.onChange(collection.collectionName, callback, streamIdx).then(
                () => { console.log("ChangeStream for " + collection.collectionName + " Re-established."); },
                () => { console.warn("ChangeStream for " + collection.collectionName + " Could Not Re-established."); }
              ); // resume watching stream at same point
            });
          resolve(true);
          return true; // For return from onCollection
        });
        connected.then(
          ()=>{},
          err => reject(err)
        );
      });
   },
   changeStreamLog: [] // Helper list of the resume_tokens for each change stream.
}
