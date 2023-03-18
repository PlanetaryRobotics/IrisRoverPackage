import Vue from 'vue';
import Vuex from 'vuex';

import DBLazyList from '@/DBInterface/DBLazyList.js';
import FTAData from '@/data_classes/FTAData.js';
import Collections from '@/DBInterface/Collections.js';

// Import Vuex Modules:
import CLI from '@/components/CLI/store.js';
import IMG from '@/components/ImageViewer/store.js';
import SAND from '@/components/Sandboxes/store.js';
import ATOMIC from '@/components/atomic/store.js';
import MAP from '@/components/Map/store.js';
import POI from '@/components/POI/store.js';

import SYS from '@/system/store.js';

Vue.use(Vuex);

export default new Vuex.Store({
    modules:{
        CLI,
        IMG,
        SAND,
        ATOMIC,
        MAP,
        POI,
        SYS
    },

    // Store Assets Unrelated to Any Major Component or too small to require their own store file:
    state: {  
    // Component header information
        headers: {
            IMG: {
                name: 'IMAGE VIEWER',
                menu: ['Close', 'Smile', 'Refresh']
            },
            CLI: {
                name: 'COMMAND LINE',
                menu: ['Close', 'Smile', 'Refresh']
            },
            MAP: {
                name: 'SHARED MAP',
                menu: ['Close', 'Smile', 'Refresh']
            },
            POI: {
                name: 'POI DASHBOARD',
                menu: ['Close', 'Smile', 'Refresh']
            }
        },
        // List of FTAData Objects Held in the Log in the Database's Commands Collection (only load the most recent entry):
        FTALogList: new DBLazyList({
            collection: Collections.FTA,
            objClass: FTAData,
            length: 1,
            staleTime: Infinity,
            autopopulate: true
        }),
        fleurInitialized: false
    },

    getters: {
    // Returns a function to force vuex to recompute on each call
    // (otherwise it uses a cached value which only changes when the
    // /value/ of state.FTALog changes):
        FTALog: state => (() => state.FTALogList.list)(),
        // Core Data of the Latest System State:
        currentFTAState: (state, getters) => {
            if(getters.FTALog.length){
                return getters.FTALog.reduce( (last, curr) => curr.data.lookupID > last.data.lookupID ? curr : last ).data;
            } else{
                return new FTAData({}).data; // Return blank / default empty FTAData.
            }
        },
    },

    mutations: {
        fleurInitialized(state) {
            state.fleurInitialized = true;
        }
    },

    actions: {

    }
});
