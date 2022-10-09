<template>
  <div class="login">
    <div class="logo-overlay">
      <transition name="logo-intro-wipe">
        <div
          v-if="logoLoaded"
          class="logo arc"
          v-html="logoSVGArc"
        />
      </transition>
      <transition name="logo-intro">
        <div
          v-if="logoLoaded"
          class="logo"
          v-html="logoSVG"
        />
      </transition>
    </div>
    <div class="input-block">
      <TextInput
        tab-order="1"
        placeholder="Server"
        :callback="collectServerName"
        :form-event="submitEvent"
        :valid-terms="standardServers"
        :valid-regex="/^.*$/i"
        :search-limit="0"
        :clear-on-enter="false"
        :case-sensitive="false"
        :hide-text="false"
        :style="{opacity: openInputBlock ? 1.0 : 0.0}"
        class="text-input server tooltip"
        title="Name or address of server, provided by MOps or GSW. Up and down arrows scroll through default names. Press right arrow to select, tab to advance."
      />
      <TextInput
        tab-order="2"
        placeholder="Mission Name"
        :callback="collectMissionName"
        :form-event="submitEvent"
        :valid-terms="missionIDs"
        :valid-regex="/^paper\d*$/i"
        :search-limit="0"
        :clear-on-enter="false"
        :case-sensitive="false"
        :hide-text="false"
        :style="{opacity: openInputBlock ? 1.0 : 0.0}"
        class="text-input mission tooltip"
        title="Up and down arrows scroll through options. Press right arrow to select, tab to advance."
      />
      <TextInput
        tab-order="3"
        placeholder="User Name"
        :callback="collectUserName"
        :form-event="submitEvent"
        :valid-terms="['test', 'FLIGHT','INCO','CAPCOM','EECOM','GNC','TELMU']"
        :valid-regex="/^.*$/i"
        :search-limit="0"
        :clear-on-enter="false"
        :case-sensitive="false"
        :hide-text="false"
        :style="{opacity: openInputBlock ? 1.0 : 0.0}"
        class="text-input user tooltip"
        title="Operator's username. Mission Operations or Ground Software should have helped you set this up."
      />
      <TextInput
        tab-order="4"
        placeholder="User Passcode"
        :callback="collectPasscode"
        :form-event="submitEvent"
        :error-event="errorEvent"
        :search-limit="0"
        :clear-on-enter="false"
        :hide-text="true"
        :style="{opacity: openInputBlock ? 1.0 : 0.0}"
        class="text-input code"
      />
      <button
        tabindex="5"
        :style="{opacity: openInputBlock ? 1.0 : 0.0}"
        class="login-button button button__brand_mute addTag__buttons--input"
        @click="$eventHub.$emit(submitEvent);"
      >
        {{ connecting ? "・ ・ ・" : "CONNECT" }}
      </button>
    </div>
  </div>
</template>

<script>
/* global __static */ // <- keep eslint from complaining about the __static directory
import path from 'path';
import fs from 'fs';

import TextInput from '@/components/atomic/TextInput.vue';
import DB from '@/DBInterface/DBInterface';

import TooltipEquip from '@/styles/TooltipEquip.js';

export default {
    name: 'Login',
    components: {
        TextInput
    },
    data(){
        return {
            submitEvent: 'submit-login', // Name of Submission Event on the Global Event Bus
            errorEvent: 'error-login', // Name of Login Error Event on the Global Event Bus
            logoSVG: '', // Inline SVG HTML for the Main Portion of the Logo
            logoSVGArc: '', // Inline SVG HTML for the Arc of the Logo
            logoLoaded: false,
            openInputBlock: false, // Flag turns true when it's time to animate in the input block
            serverName: '',
            missionName: '',
            userName: '',
            passcode: '',
            codeError: false, // Whether an error was detected with the entered code.
            connecting: false // Whether system is currently attempting to connect to the DB.
        };
    },
    computed: {
        // All Valid Mission IDs:
        missionIDs(){
            return DB.missionIDs;
        },

        // All Standard Server Addresses:
        standardServers(){
            return DB.serverNames;
        },

        allDataCollected(){
            return this.serverName && this.missionName && this.userName && this.passcode; // All have to be populated strings.
        }
    },
    watch: {
        allDataCollected: function(collected){
            if(collected){
                this.login();
            }
        }
    },
    mounted(){
        TooltipEquip(this.$el);
    
        // Tell App.vue  Login is Mounted (so, it can activate the window now):
        this.logoSVG = fs.readFileSync(path.join(__static,'./iris_logo_main.svg'), 'utf8');
        this.logoSVGArc = fs.readFileSync(path.join(__static,'./iris_logo_arc.svg'), 'utf8');

        this.$eventHub.$emit('loginMounted');
    },
    created: function() { // Adds event listeners to the global event hub
    // Wait for App.vue window to acknowledge that the window is activated and
    // visible before animating any content:
        this.$eventHub.$on('windowActivated', this.transitionInUI);
    },
    beforeDestroy: function() { // Removes event listeners from the global event hub
        this.$eventHub.$off('windowActivated', this.transitionInUI);
    },
    methods: {
        transitionInUI(){
            // Cue logo:
            setTimeout( () => {
                this.logoLoaded = true;
            }, 500);
            // Cue Input Block:
            setTimeout( () => {
                this.openInputBlock = true;
            }, 2100);
        },

        collectServerName(x){
            this.serverName = x[0];
        },
        collectMissionName(x){
            this.missionName = x[0];
        },
        collectUserName(x){
            this.userName = x[0];
        },
        collectPasscode(x){
            this.passcode = x[0];
        },

        async login(){
            console.log('[IRIS-LOGIN] Server: ', this.serverName);
            console.log('[IRIS-LOGIN] Mission: ', this.missionName);
            console.log('[IRIS-LOGIN] User: ', this.userName);

            // Attempt to Connect to DB:
            this.connecting = true;
            let connected = await DB.init(this.serverName, this.missionName, this.userName, this.passcode);
            console.log('[IRIS-LOGIN] DB Connection', connected ? 'Successful' : 'Failed');

            if(connected){
                this.$router.push('home');
            } else {
                this.$eventHub.$emit(this.errorEvent);
            }

            // Reset Code Field:
            this.passcode = '';
            this.connecting = false;
        }
    }
};
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

  $logo-box-size: 50vmin; // Size of the box containing the logo
  $logo-size: 38vmin; // Size of the logo itself

  .logo-overlay {
    grid-area: logo;
    position: relative;
    width: $logo-box-size;
    height: $logo-box-size;
  }

  $transition-duration: 4s;
  $arc-offset-angle: (3.15px / 11); // [1px=2radians] Half of the width of the wedge in which there is no arc (ie b/c the arc doesn't span 360deg)
  $logo-opacity: 0.9;
  .logo {
    opacity: $logo-opacity;
    position: absolute;
    top: 0;
    left: 0;
    width: $logo-size;
    height: $logo-size;
    transform: translate((($logo-box-size - $logo-size)/2), (($logo-box-size - $logo-size)/2));

    &.arc {
      stroke-dasharray: (3.15px - $arc-offset-angle);
      stroke-dashoffset: (3.15px - $arc-offset-angle);
    }
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

  .logo-intro-wipe-enter-active {
    animation: logo-wipe ($transition-duration/2) ease-in-out forwards;
  }
  .logo-intro-wipe-enter-to,
  .logo-intro-wipe-leave {
    stroke-dashoffset: (3.15px - $arc-offset-angle);
    opacity: 1;
  }
  .logo-intro-wipe-leave-active {
    animation: logo-wipe reverse ($transition-duration/2) ease-in-out forwards;
  }
  .logo-intro-wipe-enter,
  .logo-intro-wipe-leave-to {
    stroke-dashoffset: 0px;
    opacity: 0.1;
  }
  @keyframes logo-wipe {
    0% {
      stroke-dashoffset: 0px;
      opacity: 0.1;
    }
    50% {
      stroke-dashoffset: 0px;
      opacity: 0.1;
    }
    100% {
      stroke-dashoffset: (3.15px - $arc-offset-angle);
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
      "server mission"
      "user code"
      "launch launch";
    align-content: start;

    .text-input, .login-button{
      margin: 2.5rem;
      height: min-content;
    }

    .server {
      grid-area: server;
      opacity: 0%;
      transition: opacity $transition-duration / 2;
      transition-timing-function: linear;
      transition-delay: 0s;
    }
    .mission {
      grid-area: mission;
      opacity: 0%;
      transition: opacity $transition-duration / 2;
      transition-timing-function: linear;
      transition-delay: 0.25s;
    }
    .user {
      grid-area: user;
      transition: opacity $transition-duration / 2;
      transition-timing-function: linear;
      transition-delay: 0.5s;
    }
    .code {
      grid-area: code;
      opacity: 0%;
      transition: opacity $transition-duration / 2;
      transition-timing-function: linear;
      transition-delay: 0.75s;
    }
    .login-button{
      grid-area: launch;
      height: 4rem;
      user-select: none;
      transition: opacity $transition-duration / 2;
      transition-timing-function: linear;
      transition-delay: 1.0s;
    }
  }
</style>
