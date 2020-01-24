import Vue from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'
import $ from 'jquery'
import 'tooltipster'

import { remote } from 'electron'

// Activate All Tooltips:
$(document).ready(function(){
  $('.tooltip').tooltipster({
    theme: ['tooltipster-borderless', 'tooltipster-borderless-customized']
  });
  $('.tooltip_left').tooltipster({
    side: 'left',
    theme: ['tooltipster-borderless', 'tooltipster-borderless-customized']
  });
});

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
        evt.preventDefault();
        this.$eventHub.$emit('leftArrowListener', evt)
      }
      else if ( evt.keyCode === 39) {
        evt.preventDefault();
        this.$eventHub.$emit('rightArrowListener', evt)
      }
    },
    // Prevent normal scroll response to arrow keys (eg. in ImageViewer Timeline):
    preventNativeHorizScroll: function(evt) {
      if( evt.keyCode === 37 || evt.keyCode === 39 ){
        evt.preventDefault();
      }
    }
  },

  created: function() {
    document.addEventListener('keyup', this.keyListener);
    document.addEventListener('keydown', this.preventNativeHorizScroll);
  },

  destroyed: function() {
    document.removeEventListener('keyup', this.keyListener);
    document.addEventListener('keydown', this.preventNativeHorizScroll);
  }
}).$mount('#app')
