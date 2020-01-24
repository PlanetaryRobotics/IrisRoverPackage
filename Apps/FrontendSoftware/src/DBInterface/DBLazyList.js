'use strict';
/*
 * Allows Use of Sub-Lists from Collections to be Used Reactively in Vue. This
 * List is Used for Lazy Loading Applications where Only the Portion of a
 * Collection which is needed is loaded.
 * This DB List will load 'length' elements from 'headIdx'.
 * If 'headIdx' is Infinity, it will load the most recent 'length' elements.
 * If 'length' is Infinity, all elements in list will be loaded. 'headIdx' will be ignored.
 * If 'staleTime' is Infinity, list will be will updated only through change stream.
 *
 * NOTE: This is "lazy" implementation. The coreData does not update unless the
 * #list getter is called. Even then, it does not update instantly. Consistent
 * updates only occur if this list is being actively watched (as is the case
 * when used reactively in Vue).
 *
 * Author: Connor W. Colombo, CMU
 * Last Update: 11/8/2019, Colombo
 */
 // TODO: Allow Individual List Items to be Edited / Replaced.
 // TODO: Better #watcher Implementation than using hash (?) (simple flags won't work in case multiple agents are checking #watcher)
 // TODO: FIXME: Change streams don't currently care out headIdx or length (all changes are captured).

 import { sha256 } from 'js-sha256'

 import Collections from './Collections.js'
 import DBObject from '@/data_classes/DBObject.js'
 import DB from './DBInterface.js'

 export default class DBLazyList{
  /* Creates a New DBList.
  * collection: - Name (String or Collection Enum Symbol) of Sourcing DB Collection
  * headIdx:    - Index of First Element in the Collection to be Loaded in the List
  * length:     - Total Number of Elements to be Loaded in the List from the Collection
  * staleTime:  - Number of Seconds Until the Data is Considered to be Stale and Must be Refreshed (can be Infinity)
  * objClass:   - A Subclass of DBObject which All Elements of this List must be.
  */
  constructor(collection, headIdx, length, staleTime, objClass){
    if(Collections.contains(collection)){
      this.collection = collection;
    } else{
      this.collection = Collections.fromString(collection);
      if(!this.collection){
        console.error("Attempting to Create a DBList with an Invalid Collection: " + Collections.toString(collection));
      }
    }
      this._coreData = [];
      this._headIdx = headIdx;
      this._length = length;
      this._staleTime = staleTime;
      if(objClass.prototype instanceof DBObject){
        this.objClass = objClass;
      } else{
        this.objClass = DBObject;
      }
      this.setNextUpdateTime();
      this.needsUpdate = 1; // Whether the DB needs to be updated
      // Array of Observers Waiting to be Notified about the Next Update (see #onNextUpdate):
      this.tempObservers = [];
      // Array of Observers Waiting to be Notified about the Every Update (see #onUpdate):
      this.permObservers = [];

      this.initialLoad = 0; // Whether all data in range has been initially loaded.

      this.changeStreamConnected = false;

      // Update to Perform from Change Stream:
      this.streamUpdate = (change) => { // use lambda to preserve "this" to this instance's scope so it doesn't get re-bound by mongodb
        switch(change.operationType){
          case 'insert':{
            let doc = change.fullDocument;
            this.coreData.push(this.objClass.fromJSON(doc));
          } break;
          case 'update':{
            let doc = change.fullDocument;
            // Splice in new document in place of existing document(s) with given lookupID:
            for(let i in this.coreData){
              if(this.coreData[i].data.lookupID == doc.lookupID){
                this.coreData.splice(i, 1, this.objClass.fromJSON(doc));
              }
            }
          } break;
          case 'delete':{
            // TODO
            console.log("Document deleted from ", this.collection);
            console.log(change);
          } break;
        }
      }

      // Once DB is Connected:
      DB.eventBus.on('statusChange', ({connected}) => {
        if(connected && !this.changeStreamConnected){
          DB.onChange(this.collection, this.streamUpdate);
        }
      });
  }

  // Returns the Sub-List Taken from the Collection, Refreshing it if Necessary
  get list(){
    if(DB.connectionEstablished()){
      if(
        !this.initialLoad
      || this.staleTime!==Infinity && (this.needsUpdate || new Date() > this.nextUpdateTime)
      ){
        this.initialLoad = 1;
        let dataRead;
        if(this.length === Infinity){
          dataRead = DB.read(this.collection, false);
        } else{
          if(this.headIdx === Infinity){
            dataRead = DB.getLast(this.collection, this.length);
          } else{
            dataRead = DB.read(this.collection, {$gte: this.headIdx, $lt: this.headIdx + this.length});
          }
        }
        // Set coreData once async load has completed:
        dataRead.then( docs => { // NOTE: coreData WON'T be updated automatically
                                 // but this function will return right away with
                                 // the old value.

          // (map converts plain JSON objects into objects of the specified DBObject class)
          this.coreData = docs.map( d => this.objClass.fromJSON(d) );
        });

        // Reset the signals immediately to prevent repeated triggering while the
        // data is being async loaded. Big problem if this list is being actively
        // watched.
        this.needsUpdate = false;
        this.setNextUpdateTime();
      }

      return this.coreData;
    } else{
      return [];
    }
  }

  /*
   * Updates the underlying '_coreData' property and alerts any observers of the
   * change.
   */
  set coreData(arr){
    // Must call splice instead of simple '=' assignment to maintain Vuex reactivity:
    this._coreData.splice(0, this._coreData.length+1, ...arr);
    // Notify Registered Observers about this Update:
    this.permObservers.forEach( f => f() );
    this.tempObservers.forEach( f => f() );
    // Empty list of temporary observers:
    this.tempObservers.splice(0, this.tempObservers.length);
  }
  get coreData(){ return this._coreData; }

  /*
    Pushes the Given Object to the Database Collection this List is Connected to.
    NOTE: If the Created Index is out of Range of this List, it will not appear
    in this list.
    This event raises the special needsUpdate flag to fetch the element if it is
    in range of this list.
  */
  async push(obj){
    return new Promise( (resolve,reject) => {
      DB.write(this.collection, obj).then( (lid) => {
        this.needsUpdate = true;
        resolve(lid);
      },
      err => reject(err) // rejects when DB.write rejects
      ).catch( err => reject(err) ); // rejects when DB.write throws an error
    });
  }

  // Adds the Given Functions to the List of Functions to be
  // Called the Next Time the 'coreData' gets updated.
  onNextUpdate(fnc){
    this.tempObservers.push(fnc);
  }
  // Adds the Given Functions to the List of Functions to be
  // Called the Every Time the 'coreData' gets updated.
  onUpdate(fnc){
    this.permObservers.push(fnc);
  }

  // Set Time for Next Update Based on 'staleTime'
  setNextUpdateTime(){
    if(this.staleTime !== Infinity){
      this.nextUpdateTime = new Date(); // now
      this.nextUpdateTime.setSeconds(this.nextUpdateTime.getSeconds() + this.staleTime);
      setTimeout(()=>{this.list}, this.staleTime*1000+1); // Force a reload
    }
  }

  // Returns a Hash of the JSON of this List for Easy Reactive Watching.
  get watcher(){
    return sha256(JSON.stringify(this.list));
  }


  // Sets the Head Index and Alerts the List that it Needs to Pull From DB Again.
  set headIdx(idx){
    this._headIdx = idx;
    this.needsUpdate = 1;
  }
  get headIdx(){ return this._headIdx; }

  // Sets the Length of this List and Alerts the List that it Needs to Pull From DB Again.
  set length(n){
    this._length = n;
    this.needsUpdate = 1;
  }
  get length(){ return this._length; }

  // Sets the staleTime and Updates this.nextUpdateTime Accordingly.
  set staleTime(t){
    if(t !== Infinity){
      if(this.staleTime !== Infinity){
        this.nextUpdateTime.setSeconds(this.nextUpdateTime.getSeconds() - this.staleTime + t);
      } else{ // No current valid 'nextUpdateTime' if staleTime is Infinity.
              // So, create a new one starting from now.
        this.nextUpdateTime = new Date(); // now
        this.nextUpdateTime.setSeconds(this.nextUpdateTime.getSeconds() + t);
      }
    }
    this._staleTime = t;
  }
  get staleTime(){ return this._staleTime; }
}
