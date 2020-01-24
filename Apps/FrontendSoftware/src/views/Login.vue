<template>
  <div class="login">
    <transition name="logo-intro">
      <div v-if="logoLoaded" class="logo" v-html="logoSVG" />
    </transition>
    <div class="input-block">
        <TextInput
          placeholder="Mission Name"
          :callback="collectMissionName"
          :formEvent="submitEvent"
          :validTerms="missionIDs"
          :validRegex="/^paper\d*$/i"
          :searchLimit="0"
          :clearOnEnter="false"
          :caseSensitive="false"
          :hideText="false"
          :style="{opacity: openInputBlock ? 1.0 : 0.0}"
          class="text-input mission tooltip" title="Up and down arrows scroll through options. Press tab to select. Enter to confirm."
        />
        <TextInput
          placeholder="Mission Passcode"
          :callback="collectMissionCode"
          :formEvent="submitEvent"
          :errorEvent="errorEvent"
          :searchLimit="0"
          :clearOnEnter="false"
          :hideText="true"
          :style="{opacity: openInputBlock ? 1.0 : 0.0}"
          class="text-input code"
        />
        <TextInput
          placeholder="Operations Role"
          :callback="collectRole"
          :formEvent="submitEvent"
          :validTerms="['FLIGHT','INCO','CAPCOM','EECOM','GNC','TELMU']"
          :searchLimit="0"
          :clearOnEnter="false"
          :caseSensitive="false"
          :hideText="false"
          :style="{opacity: openInputBlock ? 1.0 : 0.0}"
          class="text-input role tooltip" title="Up and down arrows scroll through options. Press tab to select. Enter to confirm."
        />
        <button
          :style="{opacity: openInputBlock ? 1.0 : 0.0}"
          class="login-button button button__primary addTag__buttons--input"
          @click="$eventHub.$emit(submitEvent);"
        > {{ connecting ? "・ ・ ・" : "CONNECT" }}</button>
    </div>
  </div>
</template>

<script>
/* global __static */ // <- keep eslint from complaining about the __static directory
import path from 'path'
import fs from 'fs'

import TextInput from "@/components/atomic/TextInput.vue"
import DB from '@/DBInterface/DBInterface'

export default {
  name: 'login',
  data(){
    return {
      submitEvent: "submit-login", // Name of Submission Event on the Global Event Bus
      errorEvent: "error-login", // Name of Login Error Event on the Global Event Bus
      logoSVG: "", // Inline SVG HTML for the Radial Grid
      logoLoaded: false,
      openInputBlock: false, // Flag turns true when it's time to animate in the input block
      missionName: "",
      missionCode: "",
      operationsRole: "",
      codeError: false, // Whether an error was detected with the entered code.
      connecting: false // Whether system is currently attemping to connect to the DB.
    };
  },
  components: {
    TextInput
  },
  mounted(){
    // Tell App.vue  Login is Mounted (so, it can activate the window now):
    this.logoSVG = fs.readFileSync(path.join(__static,'./cuberover_logo_old.svg'), 'utf8');

    this.$eventHub.$emit('loginMounted');
  },
  created: function() { // Adds event listeners to the global event hub
    // Wait for App.vue window to acknowledge that the window is activated and
    // visible before animating any content:
    this.$eventHub.$on('windowActivated', this.transitionInUI);
  },
  beforeDestroy: function() { // Removes event listners from the global event hub
    this.$eventHub.$off('windowActivated');
  },
  methods: {
    transitionInUI(){
      // Cue logo:
      setTimeout( () => {
        this.logoLoaded = true;
      }, 1000);
      // Cue Input Block:
      setTimeout( () => {
        this.openInputBlock = true;
      }, 3500);
    },

    collectMissionName(x){
      this.missionName = x[0];
    },
    collectMissionCode(x){
      this.missionCode = x[0];
    },
    collectRole(x){
      this.operationsRole = x[0];
    },

    async login(){
      console.log("Mission: ", this.missionName);
      console.log("Role: ", this.operationsRole);

      // Attempt to Connect to DB:
      this.connecting = true;
      let connected = await DB.init(this.missionName, this.missionCode);
      console.log("DB Connection", connected ? "Successful" : "Failed");

      if(connected){
        this.$router.push('home');
      } else {
        this.$eventHub.$emit(this.errorEvent);
      }

      // Reset Code Field:
      this.missionCode = "";
      this.connecting = false;
    }
  },
  computed: {
    // All Valid Mission IDs:
    missionIDs(){
      return DB.missionIDs;
    },

    allDataCollected(){
      return this.missionName && this.missionCode && this.operationsRole; // All have to be populated strings.
    }
  },
  watch: {
    allDataCollected: function(collected){
      if(collected){
        this.login();
      }
    }
  }
}
</script>

<style scoped lang="scss">
@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_inputs.scss';

  .login{
    height: 100vh;
    width: 100%;
    display: grid;
    grid-template-columns: 1fr;
    grid-template-rows: 3fr 3fr;
    grid-template-areas:
      "logo"
      "controls";
    background-color: $color-near-black;
    justify-items: center;
    padding: .5rem;
  }

  #logoSVG {
    grid-area: logo;
    align-item: center;
    width: 50vmin;
    height: 50vmin;
    svg {
      width: 100%;
      height: 100%;
    }
  }

  $transition-duration: 5s;
  $logo-opacity: 0.7;
  .logo {
    opacity: $logo-opacity;
  }

  .logo-intro-enter-active {
    animation: logo-fade-in $transition-duration;
  }
  .logo-intro-leave-active {
    animation: logo-fade-in reverse $transition-duration;
  }
  @keyframes logo-fade-in {
    0% {
      opacity: 0;
    }
    100% {
      opacity: $logo-opacity;
    }
  }

  .input-block {
    margin-top: 0rem;
    grid-area: controls;
    width: calc(33% + 5rem);
    display: grid;
    grid-template-columns: minmax(0,1fr) minmax(0,1fr);
    grid-template-rows: min-content min-content 4rem auto;
    grid-template-areas:
      "mission code"
      "role role"
      "launch launch";
    align-content: start;

    .text-input, .login-button{
      margin: 2.5rem;
      height: min-content;
    }
    .mission {
      grid-area: mission;
      opacity: 0%;
      transition: opacity $transition-duration / 2;
      transition-timing-function: linear;
      transition-delay: 0s;
    }
    .code {
      grid-area: code;
      opacity: 0%;
      transition: opacity $transition-duration / 2;
      transition-timing-function: linear;
      transition-delay: 0.25s;
    }
    .role {
      grid-area: role;
      transition: opacity $transition-duration / 2;
      transition-timing-function: linear;
      transition-delay: 0.5s;
    }
    .login-button{
      grid-area: launch;
      height: 4rem;
      user-select: none;
      transition: opacity $transition-duration / 2;
      transition-timing-function: linear;
      transition-delay: 0.75s;
    }
  }
</style>
