import Vue from 'vue'
import Vuex from 'vuex'

// Import Vuex Modules:
import CLI from '@/components/CLI/store.js'
import IMG from '@/components/ImageViewer/store.js'

Vue.use(Vuex)

export default new Vuex.Store({
  modules:{
    CLI,
    IMG
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
