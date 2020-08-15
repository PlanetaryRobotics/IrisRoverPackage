'use strict';
/*
 * Set of All Functionality for Pushing or Pulling JSON Data to/from the Database.

 * Author: Sofia Hurtado, CMU
 * Last Update: 5/11/2019, Colombo
 */
 // TODO: Let user w/o permissions push command to db but have it flagged and ignored. (this behavior should be moved to CommandField.vue)

import mongo from 'mongodb'
import Collections from './Collections'
import DBObject from '@/data_classes/DBObject.js'

const DB_URL = "mongodb+srv://CubeRoverAdmin:RedRover@devcluster-3thor.mongodb.net/test?retryWrites=true";
const DB_PARTITION = "test";


export default {
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
  */
  onClient: async function(command){
    let MongoClient = mongo.MongoClient;
    return new Promise( (resolve,reject) => {
      MongoClient.connect(DB_URL, {useNewUrlParser: true}, (err,client) => {
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
          let collection = client.db(DB_PARTITION).collection(Collections.toString(c));
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
   }
}
