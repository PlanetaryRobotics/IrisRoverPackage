import Vue from 'vue'
import Vuex from 'vuex'

// Import Vuex Modules:
import CLI from '@/components/CLI/store.js'
import IMG from '@/components/ImageViewer/store.js'
import SAND from '@/components/Sandboxes/store.js'
import ATOMIC from "@/components/atomic/store.js"
import MAP from '@/components/Map/store.js'

import SYS from '@/system/store.js'

Vue.use(Vuex)

export default new Vuex.Store({
  modules:{
    CLI,
    IMG,
    SAND,
    ATOMIC,
    MAP,
    SYS
  },

  // Store Assets Unrelated to Any Major Component:
  state: {
    
  },

  getters: {

  },

  mutations: {

  },

  actions: {

  }
})
