import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'

import { remote } from 'electron'

/* Whether the Application is Currently being Run in Development Mode. Any
experimental features can be wrapped in v-if="$isDev" and experimental functions
in if(this.$isDev){} to keep them from being visible in production builds: */
Vue.prototype.$isDev = remote.process.env.NODE_ENV !== 'production';

Vue.prototype.$eventHub = new Vue(); // Creates global eventHub

// Activate Vue:
new Vue({
  router,
  store,
  render: h => h(App),

  methods: {
    // Reads event inputs from body and publishes to eventHub
    keyListener: function(evt) {
      if ( evt.keyCode === 37) {
        this.$eventHub.$emit('GLOBAL-keypress-left', evt)
      }
      else if ( evt.keyCode === 39) {
        this.$eventHub.$emit('GLOBAL-keypress-right', evt)
      }
    }
  },

  created: function() {
    document.addEventListener('keyup', this.keyListener);
  },

  destroyed: function() {
    document.removeEventListener('keyup', this.keyListener);
  }
}).$mount('#app')
