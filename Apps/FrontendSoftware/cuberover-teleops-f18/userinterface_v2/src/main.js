import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import DB from '@/DBInterface/DBInterface'

DB.verifyCollectionFormatting(); // Do this once on startup

Vue.config.productionTip = false

Vue.prototype.$eventHub = new Vue(); // Creates global eventHub

new Vue({
  router,
  store,
  render: h => h(App),

  methods: {
    // Reads event inputs from body and publishes to eventHub
    keyListener: function(evt) {
      if ( evt.keyCode === 37) {
        evt.preventDefault();
        this.$eventHub.$emit('leftArrowListener', evt)
      }
      else if ( evt.keyCode === 39) {
        evt.preventDefault();
        this.$eventHub.$emit('rightArrowListener', evt)
      }
    }
  },

  created: function() {
    document.addEventListener('keydown', this.keyListener);
  },

  destroyed: function() {
    document.removeEventListener('keydown', this.keyListener);
  }
}).$mount('#app')
