import Vue from 'vue'
import Vuex from 'vuex'

// Import Vuex Modules:
import CLI from '@/components/CLI/store.js'
import IMG from '@/components/ImageViewer/store.js'
import SAND from '@/components/Sandboxes/store.js'
import ATOMIC from "@/components/atomic/store.js"
import MAP from '@/components/Map/store.js'
import POI from '@/components/POI/store.js';

Vue.use(Vuex)

export default new Vuex.Store({
  modules:{
    CLI,
    IMG,
    SAND,
    ATOMIC,
    MAP,
    POI
  },

  // Store Assets Unrelated to Any Major Component:
  state: {
    // Component header information
    headers: {
      IMG: {
        name: "IMAGE VIEWER",
        menu: ["Close", "Smile", "Refresh"]
      },
      CLI: {
        name: "COMMAND LINE",
        menu: ["Close", "Smile", "Refresh"]
      },
      MAP: {
        name: "SHARED MAP",
        menu: ["Close", "Smile", "Refresh"]
      },
      POI: {
        name: "POI DASHBOARD",
        menu: ["Close", "Smile", "Refresh"]
      }
    }
  },

  getters: {

  },

  mutations: {

  },

  actions: {

  }
})
