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
 * Last Update: 08/14/2020, Colombo
 */
 // TODO: FIXME: Allow for removal from onUpdate (possibly just replace impl. with normal node eventBus) and then find all usages of onUpdate and add associated remove in destructor of subscriber (where applicable).
 // TODO: Allow Individual List Items to be Edited / Replaced.
 // TODO: Better #watcher Implementation than using hash (?) (simple flags won't work in case multiple agents are checking #watcher)
 // TODO: FIXME: Change streams don't currently care about headIdx or length (all changes are captured).
 // TODO: Account for 'delete' changes (from ChangeStream).

 import { sha256 } from 'js-sha256'

 import Collections from './Collections.js'
 import DBObject from '@/data_classes/DBObject.js'
 import DB from './DBInterface.js'

 export default class DBLazyList{
  /* Creates a New DBList.
  * collection:   - Name (String or Collection Enum Symbol) of Sourcing DB Collection
  * objClass:     - A Subclass of DBObject which All Elements of this List must be.
  * headIdx:      - Index of First Element in the Collection to be Loaded in the List
  * length:       - Total Number of Elements to be Loaded in the List from the Collection
  * staleTime:    - Number of Seconds Until the Data is Considered to be Stale and Must be Refreshed (can be Infinity)
  * autopopulate: - Whether to autopopulate the DB collection to ensure that there's always at least one entry in it.
  */
  constructor({
    collection,
    objClass=DBObject,
    headIdx=Infinity, // Default: Grab to end
    length=Infinity, // Default: Grab everything
    staleTime=10,  // Default: Autoupdate every 10s in addition to ChangeStream calls
    autopopulate=false
  } = {}){

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
    this.needsUpdate = 1; // Whether the DB needs to be updated
    // Array of Observers Waiting to be Notified about the Next Update (see #onNextUpdate):
    this.tempObservers = [];
    // Array of Observers Waiting to be Notified about the Every Update (see #onUpdate):
    this.permObservers = [];

    this.initialLoad = 0; // Whether all data in range has been initially loaded.

    this.changeStreamConnected = false;

    // Once DB is Connected:
    // Attempt to setup ChangeStream based on given DB connection status:
    const attemptChangeStreamSetup = ({connected}) => {
      if(connected && !this.changeStreamConnected){
        DB.eventBus.off('statusChange', attemptChangeStreamSetup);
        DB.onChange(this.collection, this.streamUpdate, Infinity, ({connected}) => {this.changeStreamConnected = connected;});
      }
    };
    DB.eventBus.on('statusChange', attemptChangeStreamSetup);

    // Attempt to load data until successful based on the given database connection status:
    const attemptInitialDataLoad = ({connected}) => {
      if(connected){
        DB.eventBus.off('statusChange', attemptInitialDataLoad); // Unregister from eventBus
        this.updateData(); // Initial data load
      }
    }
    DB.eventBus.on('statusChange', attemptInitialDataLoad);

    this.autopopulated = false;
    if(autopopulate){
      // Once DB is Connected, make sure there is at least one object in the collection:
      DB.eventBus.on('statusChange', this.autopopulateProcess);
    }
    
    DB.checkConnection(); // Force initial connection check.
  }

  /**
   * Special option. Autopopulates the collection with at least one entry.
   */
  autopopulateProcess = ({connected}) => {
      if(connected && !this.autopopulated){
        this.forceReactiveUpdate().then( ({data}) => {
            /** 
             * On first connection to DB, check to see if this terminal is the first to access the DB for this mission (partition).
             * If so, initialize the DB by providing an initial DataEntry entry.
             */
            DB.eventBus.off('statusChange', this.autopopulateProcess); // Deregister. Event handler no longer needed.
            if(data.length === 0){
                this.forcePush({obj: new this.objClass()});
            }
        }).catch( () => {
          // If failed to load data, retry:
          this.autopopulated = false;
          this.autopopulateProcess({connected});
        });
        this.autopopulated = true;
    }
  }

  /**
   * Update Data in response to Change Stream.
   * 
   * NOTE: Using lambda to preserve "this" to this instance's scope so it doesn't get re-bound by mongodb
   */ 
  streamUpdate = (change) => {
    switch(change.operationType){
      case 'insert':{
        // TODO: Make sure inserted document falls in range and then remove any documents that are now out of range.
        let doc = change.fullDocument;
        this.coreData.push(this.objClass.fromJSON(doc));
        this.dataUpdateRoutine();
      } break;
      case 'update':{
        let doc = change.fullDocument;
        // Splice in new document in place of existing document(s) with given lookupID:
        for(let i in this.coreData){
          if(this.coreData[i].data.lookupID == doc.lookupID){
            this.coreData.splice(i, 1, this.objClass.fromJSON(doc));
          }
        }
        this.dataUpdateRoutine();
      } break;
      case 'replace':{
        let doc = change.fullDocument;
        // Splice in new document in place of existing document(s) with given lookupID:
        for(let i in this.coreData){
          if(this.coreData[i].data.lookupID == doc.lookupID){
            this.coreData.splice(i, 1, this.objClass.fromJSON(doc));
          }
        }
        this.dataUpdateRoutine();
      } break;
      case 'delete':{
        // TODO: FIXME: BUILDME
        console.log("Document pseudo-deleted from ", this.collection);
        console.log(change);
        this.dataUpdateRoutine();
      } break;
    }
  }

  /**
   * Checks if a data update is needed and, if so, updates the `coreData`.
   * Returns a Promise that resolves/rejects when the update / update attempt is complete.
   * Resolves with:
   *  {
   *    `data`: a copy of the `coreData`,
   *    `updated`: a flag indicating whether the data was updated.
   *  }
   * Rejects if there's an error during update with a copy of the error.
   */
  async updateData(forceUpdate=false){
    return new Promise( (resolve, reject) => {
      if(
        !this.initialLoad
      || this.staleTime!==Infinity && (this.needsUpdate || new Date() > this.nextUpdateTime)
      || forceUpdate
      ){
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
          this.initialLoad = 1;
          // (map converts plain JSON objects into objects of the specified DBObject class)
          this.coreData = docs.map( d => this.objClass.fromJSON(d) );
          resolve({data: this.coreData, updated: true});
        }).catch( (err) => {
          reject(err)
        });

        // Reset the signals immediately to prevent repeated triggering while the
        // data is being async loaded. Big problem if this list is being actively
        // watched.
        this.needsUpdate = false;
        this.setNextUpdateTime();
      } else{
        resolve({data: this.coreData, updated: false});
      }
    });
  }

  // Returns the Sub-List Taken from the Collection, Refreshing it if Necessary
  get list(){
    if(DB.connectionEstablished()){
      this.updateData();
      return this.coreData;
    } else{
      return [];
    }
  }


  get coreData(){ return this._coreData; }
  /*
   * Updates the underlying '_coreData' property and alerts any observers of the
   * change.
   */
  set coreData(arr){
    // Must call splice instead of simple '=' assignment to maintain Vuex reactivity:
    this._coreData.splice(0, this._coreData.length+1, ...arr);
    this.dataUpdateRoutine();
  }
  // Tasks to Perform Every Time Data is Updated:
  dataUpdateRoutine(){
    this.callObservers();
  }
  // Notify Registered Observers about a Data Update:
  callObservers(){
    // Notify Registered Observers about this Update:
    this.permObservers.forEach( f => f() );
    this.tempObservers.forEach( f => f() );
    // Empty list of temporary observers:
    this.tempObservers.splice(0, this.tempObservers.length);
  }

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
        resolve(lid);
        this.forceReactiveUpdate();
      },
      err => reject(err) // reject when DB.write rejects
    ).catch( err => reject(err) ); // reject when DB.write throws an error
    });
  }  

  /**
   * @callback dbForcePushCallback
   * @param {number} [uuid] UUID for the DBObject being pushed
   * @param {number} [lid] Newly minted lookupID for DBObject on the DB
   */

  /**
   * @callback dbForcePushFailCallback
   * @param {number} [uuid] UUID for the DBObject being pushed
   */

  /**
   * Same as `push` but, if it fails, it keeps retrying until the data is successfully 
   * pushed or an option `maxAttempts` or `maxDelay` is exceeded.
   * Note: despite any limits imposed by `maxDelay`, there will always be at least 1 attempt; however, if `maxAttempts` is <1, nothing will be attempted.
   * @param {DBObject} obj Object to push.
   * @param {number} [maxAttempts] Inclusive maximum number of attempts (default is Infinity)
   * @param {number} [maxDelay] [ms] Maximum time from `startTime` to spend trying to push data (default is Infinity)
   * @param {number} [attemptInterval] [ms] Time between attempts (when failure not due to a DB connection issue)
   * @param {dbForcePushFailCallback} [onFirstFail] Callback after the first attempt fails
   * @param {dbForcePushFailCallback} [onFail] Callback when an attempt fails
   * @param {dbForcePushFailCallback} [onFinalFail] Callback after the last attempt fails (if applicable)
   * @param {dbForcePushCallback} [onSuccess] Callback when the object is successfully pushed to the DB
   * 
   * NOTE: The following parameters are used purely in recursive calls and should not need to be supplied by external callers:
   * @param {Date} [startTime] Reference time to measure operating delay with respect to (don't need to populate this externally. It will get populated once on first call and then carry over to all successive recursive calls via this argument)
   * @param {Boolean} [firstAttempt] Whether this is the first attempt. (don't need to populate this externally. It will get populated once on first call and then carry over to all successive recursive calls via this argument)
   */
  forcePush(args){
    const {
      obj,
      maxAttempts=Infinity,
      maxDelay=Infinity,
      attemptInterval=1000,
      onFirstFail=()=>{},
      onFail=()=>{},
      onFinalFail=()=>{},
      onSuccess=()=>{},
      startTime=new Date(),
      firstAttempt=false
    } = args || {};
    
    if(maxAttempts < 1){
      console.error(`
        Error in LazyList forcePush on ${this.collection}.
        \`maxAttempts\` can't be < 1. It's ${maxAttempts}.`
      );
    }

    if(maxDelay < 0){
      console.error(`
        Error in LazyList forcePush on ${this.collection}.
        \`maxDelay\` can't be < 0. It's ${maxDelay}.`
      );
    }

    if(attemptInterval < 0){
      console.error(`
        Error in LazyList forcePush on ${this.collection}.
        \`attemptInterval\` can't be < 0. It's ${attemptInterval}.`
      );
    }

    if(maxAttempts < 1){
      return; // Don't attempt if no attempts left
    }

    this.push(obj).then(
      // Handle Success:
      (lid) => { onSuccess(obj.uuid, lid) }
    ).catch(
      // Cope with Failure:
      () => {
        if(firstAttempt){
          onFirstFail(obj.uuid);
        }
        onFail(obj.uuid);
        if(maxAttempts > 1 && (new Date() - startTime) <= maxDelay){
          // If this can be reattempted, check why this failed then retry accordingly:

          // Setup a call for another attempt but with fewer attempts remaining
          // (Note: since `startTime` is preserved, nothing has to be done with `maxDelay`)
          const nextArgs = Object.assign({}, args);
          nextArgs.firstAttempt = false;
          nextArgs.maxAttempts--;

          // Check connection status:
          DB.checkConnection().then( connectionStatus => {
            if(!connectionStatus){
              // If failure was likely due to a connection failure, then wait for DB to reconnect then push:
              const waitForDBConnect = ({connected}) => {
                if(connected){
                  DB.eventBus.off('statusChange', waitForDBConnect); // unregister since this only needs to be a one time thing.
                  this.forcePush(nextArgs); // "recurse" after unregistering to prevent memory leak (stacked buildup of connection requests)
                }
              };
              DB.eventBus.on('statusChange', waitForDBConnect);
            } else{
              // Otherwise, just try again in `attemptInterval` ms:
              setTimeout(this.forcePush(nextArgs), attemptInterval);
            }
          });
        } else {
          // Otherwise, this failed for the last time.
          onFinalFail(obj.uuid);
        }
      }
    );
  }

  /**
   * Force a reactive update (makes Vue notice the update sooner).
   * Useful for fetching new content after a write.
   * Returns the promise from `updateData`.
   */
  forceReactiveUpdate(){
    this.needsUpdate = true;
    const promise = this.updateData(1); // Force update
    this.list; // Make sure Vue notices
    return promise;
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
      setTimeout(()=>{this.updateData()}, this.staleTime*1000+1); // Force a reload
    }
  }

  /** Returns a Hash of the JSON of this List for Easy Reactive Watching.
   * NOTE: Only use in very rare circumstances. It is highly advised that 
   * implementations use `onUpdate` instead.
   */
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
