# This script sends a json file to a target MongoDB database
# Input: f_name - path/name of the json file
#        client_url - url of the MongoDB client
#        db_name - name of the database
#        collection_name - name of the collection
# Output: N/A

import json
from pymongo import MongoClient

def json2mongodb(f_name,client_url,db_name,collection_name):
    client = MongoClient(client_url) # e.g 'mongodb://localhost:27017/'
    db = client[db_name]
    collection = db[collection_name]

    with open(f_name) as f:
    	file_data = json.load(f)

    collection.insert(file_data)
    client.close()