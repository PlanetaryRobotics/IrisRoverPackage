/*
 * Vuex Module for Splitting out the Aspects of the Central Store owned by the
 * System Manager
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/07/2020
 * Last Updated: 08/14/2020, Colombo
 */

import DBLazyList from '@/DBInterface/DBLazyList.js';
import SystemData from '@/data_classes/SystemData.js';
import Collections from '@/DBInterface/Collections.js';

export default {
    state: {
    // List of SystemData Objects Held in the Log in the Database's Commands Collection (only load the most recent entry):
        stateLogList: new DBLazyList({
            collection: Collections.System,
            objClass: SystemData,
            headIdx: Infinity,
            length: 1,
            staleTime: Infinity,
            autopopulate: true
        })
    },

    mutations: {

    },

    getters: {
    // Returns a function to force vuex to recompute on each call
    // (otherwise it uses a cached value which only changes when the
    // /value/ of state.SYS.stateLog changes):
        systemStateLog: state => (() => state.stateLogList.list)(),
        // Core Data of the Latest System State:
        currentSystemState: (state, getters) => {
            if(getters.systemStateLog.length){
                return getters.systemStateLog.reduce( (last, curr) => curr.data.lookupID > last.data.lookupID ? curr : last ).data;
            } else{
                return new SystemData({}).data; // Return blank / default empty SystemData.
            }
        },
    }
};
