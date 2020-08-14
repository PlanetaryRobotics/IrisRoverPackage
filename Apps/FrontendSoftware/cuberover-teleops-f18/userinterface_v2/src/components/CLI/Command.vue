<!--
A single instance of a command to be executed, whose type is given by the name
of a CommandOption, such as "TurnLeft".
for all the parameters.

TODO:
  - Convert strings back to numbers before sending.
  - Add toggle for making a command sticky (?)

Author: Connor W. Colombo, CMU
Created: 1/24/2019
Last Updated: 5/11/2019, Colombo
-->
<template>
  <div ref="container" class="command-container">
    <div class="command" v-on:click="showInfo = !showInfo;">
      <div ref="content" class="content">
        <p ref="commandTag" class="command-tag" v-html="displayString"></p>
        <span v-bind:style="{visibility: showInlineData ? 'visible' : 'hidden'}">
          <p class="command-time" v-if="sent||failed"> {{ sendTime }} </p>
          <p class="command-size" v-if="!sent&&!failed"> {{ byteSize }} bytes</p>
        </span>
      </div>
      <div class="status" v-bind:class="stateClass"></div>
    </div>
    <!-- ERROR MESSAGE -->
    <transition name="notice-in" enter-active-class="animated slideInDown" leave-active-class="animated slideOutUp">
      <div class="notice" v-if="!sent" v-bind:class="stateClass" v-html="notice"></div>
    </transition>
    <!-- COMMAND DESCRIPTION -->
    <transition name="info-in" enter-active-class="animated slideInDown" leave-active-class="animated slideOutUp">
      <div class="info" v-if="showInfo">
        <b>{{ commandOption.name }} [ <span v-for="(param,i) in commandOption.params" v-bind:key="i">{{i!=0 ? ',' : ""}}&nbsp;{{param.name}}</span> ]</b>
        <br />
        <span> {{ commandOption.desc }} </span>
      </div>
    </transition>
  </div>
</template>

<script>
import { mapState } from 'vuex'
import Vue from 'vue'
import VTooltip from 'v-tooltip'

import States from '@/data_classes/CommandStates.js'
import Strings from '@/data_classes/CommandStrings.js'
import CommandData from '@/data_classes/CommandData.js'

Vue.use(VTooltip);

export default{
  name: 'Command',
  props: {
    core: { // - Core Command Data of this List
      type: CommandData,
      required: true
    }
  },
  data(){
    return {
      commandOption: {}, // - Fetched CommandOption Object
      displayString: "", // - Cosmetic Presentation of Command (for HTML)
      commandString: "", // - Pure Text Representation of Command (for Logging)
      showInfo: false, //   - Whether info about this command should be displayed
      showInlineData: true //-Whether inline data (such as send time) should be displayed
    };
  },
  computed: {
    ...mapState({
      commandOptions: state => state.CLI.CommandOptions
    }),

    // Name of the CommandOption this Represents
    commandOptionName(){
      return this.core.data.name;
    },

    // Arguments of the Core Command as an array of valuess:
    args(){
      return Object.values(this.core.data.args);
    },

    // Whether the message has been successfully sent to the Rover
    sent(){
      return this.core.data.stateUI === States.UI.SUCCESS;
    },

    // Whether the command failed at any stage
    failed(){
      return this.core.data.stateFp === States.Fp.FAIL
          || this.core.data.stateUI === States.UI.FAIL;
    },

    // Whether this message still only exists on this machine (has not yet been
    // successfully pushed to the DB):
    localOnly(){
      return this.core.data.stateUI === States.UI.LOCAL_ONLY;
    },

    // Time at which the message was successfully sent (Date Object)
    timestamp(){
      return this.core.data.sendTime;
    },

    // Returns a Formatted String of the Time the Message was Sent:
    sendTime(){
      return this.timestamp.toLocaleDateString('en-US', { timezone: 'UTC', hour12 : false, month: '2-digit', day: '2-digit', hour: '2-digit', minute: '2-digit', second: '2-digit'});
    },

    // Current State of this Command as Legible Text:
    currentState(){
      // NB: 'Waiting' could be either waiting to be sent or waiting for response. Consider bifurcating.
      return this.sent ? 'sent successfully' : this.failed ? 'failed' : 'waiting';
    },

    // Returns a Class Object which Assigns a Class, Based on the Current Command State:
    stateClass(){
      return {
        success: this.sent,
        fail: this.failed,
        local: this.localOnly,
        waiting: !this.sent&&!this.failed&&!this.localOnly
      };
    },

    // Any notices to the user, based on either the state
    notice(){
      return this.sent ? ""
           : this.failed ? `<b>FAILURE:</b> ${this.core.data.stateFp === States.Fp.FAIL ? `<br />Rover: ${Strings[this.core.data.errorFp]}` : ""} ${this.core.data.stateUI === States.UI.FAIL ? `Interface: ${Strings[this.core.data.errorUI]}` : ""}`
           : this.localOnly ? "This Command is <b>Local Only</b>. It is currently waiting to be pushed to the database server from which it will be forwarded to the Rover. <br /> Until this command is on the database, it risks being lost if this program closes."
           : "Waiting for Completion.";
    },

    // Compute Minimum Size of Command in Bytes (REALLY ROUGH):
    // TODO: Make this much more accurate.
    byteSize(){
      // 8bytes for int, 32 for float:
      return this.args.map( x => x%1==0 ? 8 : 32 ).reduce((acc,val) => acc + val);
    }
  },
  mounted(){
    // Initialize frequently used or lengthy to compute data:
    this.commandOption = this.commandOptions.find( c => c.name == this.commandOptionName );

    this.displayString = `${this.commandOption.name}&nbsp;&nbsp;[`;
    this.commandString = `${this.commandOption.name}&nbsp;&nbsp;[`;
    this.args.forEach((val,i) => {
      if(i > 0){
        this.displayString += ',';
        this.commandString += ',';
      }
      this.displayString += '&nbsp;&nbsp;' + val + this.commandOption.formattedUnits[i];
      this.commandString += '&nbsp;&nbsp;' + val + this.commandOption.params[i].units;
    });
    this.displayString += "&nbsp;&nbsp;]";
    this.commandString += "&nbsp;&nbsp;]";

    // Perform Basic Error Checks:
    if( this.commandOption.params.length !== this.args.length ){
      console.error("Mismatch in Number of Arguments Provided to Command.");
    }
  }
}
</script>

<style lang="scss" scoped>
  @import '@/styles/animate.scss';
  @import '@/styles/_colors.scss';

  .success{
    color: $color-near-white;
    background-color: $color-nominal;
  }
  .fail{
    color: $color-near-white;
    background-color: $color-danger;
  }
  .waiting{
    color: $color-near-white;
    background-color: $color-caution;
  }
  .local{
    color: $color-near-white;
    background-color: adjust-hue($color-caution, -20%);
  }

  /* Command has been marked to stay at the top */
  .sticky{
    position: sticky;
    top: 1em;
  }

  .command-container{
    user-select: none;
    border-radius: .6rem;
    margin: 1rem;
    overflow: auto;
    /* Keep from being compressed*/
    margin-bottom: 7px;
    min-width: calc(auto + 3ch);
  }

  .command{
    background-color: $color-grey-dark;
    color: $color-near-white;
    position: relative;
    z-index: 50;
    display: grid;
    grid-template-rows: 1fr 4px;
    grid-template-areas:  "content"
                          "status";
  }
  .content{
    grid-area: content;
    padding: 1rem 1rem;
  }
  .command-tag{
    line-height: 50%;
    font-weight: normal;
    float: left;
  }
  .command-time{
    margin-left: 3ch;
    float: right;
  }
  .command-size{
    margin-left: 3ch;
    float: right;
  }

  .status{
    grid-area: status;
  }

  .notice, .info{
    position: relative;
    z-index: 10;
    border-radius: 0 0 4px 4px;
    padding: 5px 0 5px 4%;
    text-align: left;
  }

  .notice{
    padding: 1rem 1rem;
    line-height: 175%;
    background-color: $color-near-black;
  }

  .info{
    background-color: $color-near-black;
    color: $color-near-white;
    padding: 1rem 1rem;
  }

</style>
