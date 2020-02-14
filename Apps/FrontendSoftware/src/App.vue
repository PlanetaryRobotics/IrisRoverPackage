<template>
  <div id="app" class="text__main">
    <router-view/>
  </div>
</template>

<script>
import { remote } from 'electron'
import { reportReady } from 'electron-splashscreen'
import DB from '@/DBInterface/DBInterface'

export default {
  name: 'App',
  components: {
  },
  created: function() {
    // Add event listners to the global event hub:
    this.$eventHub.$on('loginMounted', this.activateWindow);

    this.processArgs();
  },
  beforeDestroy: function() { // Removes event listners from the global event hub
    this.$eventHub.$on('loginMounted')
  },
  methods: {
    activateWindow: function(){
      /* Wrapped in timeout to add semi-arbitrary delay (needs to be greater
      than ~500ms to avoid flashing white screen when opening to fullscreen.
      Can't prevent this by listening for DOM load because Vue will still be
      bringing in resources after first load).
      That said, there almost certainly is a more robust implementation but
      simply not worth the dev time or maintenance (would likely require
      reporting from all components loaded in Login.vue and router). */
      setTimeout(()=>{
        reportReady(); // App is loaded, close the splash-screen
        if(process.env.NODE_ENV == 'production'){
          remote.getCurrentWindow().setFullScreen(true);
        }
        this.$eventHub.$emit('windowActivated');
      }, 1500);
    },

    processArgs: async function(){
      // Process command-line arguments:
      let argv = JSON.parse(remote.process.env.npm_config_argv).remain; // grab unused arguments
      let keys = argv.filter((v,i) => !(i % 2)); // grab keys
      let vals = argv.filter((v,i) => i % 2); // grab values associated with keys

      // Login to the DB (skipping UI login). MUST happen before any Vue routing
      // to pages that require DB access.
      let missionIdx = keys.indexOf("db-mission"); // Index of db-mission value
      if(missionIdx != -1){
        let mission = vals[missionIdx];
        let codeIdx = keys.indexOf("db-pass");
        let code = codeIdx > -1 ? vals[codeIdx] : "";

        let connected = await DB.init(mission, code);
        console.log("DB Connection", connected ? "Successful" : "Failed");
      }

      // Handle the rest of the keys in any order:
      for(let i in keys){
        switch(keys[i]){

          case "route-to": { //     - Route to a specific page in views.
            this.$eventHub.$emit('loginMounted'); // skip Login
            try{
              console.log("Routing to " + vals[i] + " . . .");
              this.$router.push(vals[i]);
            } catch(e){
              console.warn(e);
              console.log("Page " + vals[i] + " not registered in router.js");
            }
          } break;

        }
      }
    }
  }
}
</script>

<style lang="scss">
@import '/styles/_functional.scss';
@font-face {
  font-family: 'Barlow';
  src: url('./styles/Barlow/Barlow-Regular.ttf')  format('truetype');
  font-weight: normal;
  font-style: normal;
}
@font-face {
  font-family: 'Barlow';
  src: url('./styles/Barlow/Barlow-SemiBold.ttf')  format('truetype');
  font-weight: bold;
  font-style: normal;
}
@font-face {
  font-family: 'Barlow';
  src: url('./styles/Barlow/Barlow-Italic.ttf')  format('truetype');
  font-weight: normal;
  font-style: italic;
}

body {
  background-color: $color-background;
  margin: 0;
  width: 100vw;
  height: 100vh;
}

*,
*::before,
*::after {
  margin: 0;
  padding: 0;
  box-sizing: inherit;
}

html {
  font-family: 'Barlow';
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: left;
  color: $color-near-black;

  box-sizing: border-box;
  font-size: 62.5%;
}

#app {
}
</style>
