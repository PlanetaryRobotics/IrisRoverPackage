<!--
Field for a Command to be Entered, which includes autocomplete functionality
which adapts to a user's most used commands.

TODO:
  - Input validation (numbers only or text only where applicable as set in file)
  - Limit number of commands which can be in waiting (THIS SHOULD BE 1 OTHERWISE ORDER IS NOT GUARANTEED)
  - User permission checking

Author: Connor W. Colombo, CMU
Created: 1/31/2019
Last Updated: 4/28/2019, Colombo
-->
<template>
  <div class="command-field">
    <input
      name="text-input"
      class="command-input"
      type="text"
      v-model="inputText"
      ref="input"
      v-on:keydown.tab.prevent="advanceInputState"
      v-on:keydown.right.prevent="advanceInputState"
      v-on:keydown.left.prevent="prevArgument"
      v-on:keydown.up.prevent="commandSuggestionNum++"
      v-on:keydown.down.prevent="commandSuggestionNum--"
      v-on:keydown.delete.prevent="backspacePressed"
      v-on:keydown.enter.prevent="sendCommand"
      v-on:keydown.esc.prevent="reset"
      @input="keyPressed"
      @click="fieldFocused = true"
      @blur="fieldFocused = false"
      spellcheck="false"
    />
    <p class="command-text" v-bind:class="{highlight: fieldFocused}">
      <span class="inputText">&nbsp;>&nbsp;{{ inputText }}</span>
      <span class="suggested">{{ suggestedText }}</span>
      <span v-if="inputText!=''" v-bind:class="{suggested: !commandSelected, complete: commandSelected}">&nbsp;[</span>
      <span v-for="(arg,idx) in currentArgs" v-bind:key="idx">
        {{ idx > 0 ? ',' : '' }}&nbsp;
        <span v-bind:class="{complete: arg!=='', suggested: arg==='', focused: idx == focusIdx}">
          {{ arg=="" ? currentCommand.params[idx].defaultVal : arg }}<span v-html="currentCommand.formattedUnits[idx]"></span>
        </span>
      </span>
      <span v-if="inputText!==''" v-bind:class="{suggested: focusIdx<currentArgs.length-1 || !commandSelected, completed: focusIdx==(currentArgs.length-1)}">&nbsp;]&nbsp;</span>
    </p>
  </div>
</template>

<script>
import { mapState } from 'vuex'

import CommandOption from '@/data_classes/CommandOption.js'
import CommandData from '@/data_classes/CommandData.js'

export default {
  name: 'CommandField',
  data(){
    return {
      currentCommand: {}, //      - Current Command Option Chosen
      currentArgs: [], //         - Arguments Given for Command
      commandSuggestionNum: 0, // - Index which Allows for Cycling Through Commands
      focusIdx: 0, //             - Index of Argument which Currently Holds Focus
      inputText: "", //           - Input by the User into the Text Field
      fieldFocused: false, //     - Whether the User's Focus is Actively on the Input
      searchActive: false //      - Whether Command Field is Actively serving as a Search for the Log.
    };
  },
  computed: {
    ...mapState({
      commandOptions: state => state.CLI.CommandOptions
    }),

    // Returns the Remainder of the Suggested Command (not currently input by
    // the user).
    suggestedText(){
      let cmd = this.suggestion(this.inputText, this.commandSuggestionNum);
      return this.isCmd(cmd) ? cmd.name.substring(this.inputText.length) : "";
    },

    // Returns Whether a Command Option is Currently Selected:
    commandSelected(){
      return this.isCmd(this.currentCommand);
    }
  },
  methods: {
    // Identifies if the Given Object is a Valid Command Object:
    isCmd(c){
      return c instanceof CommandOption;
    },

    // Sets the Current Command Option to the Currently Suggested Command:
    selectCommand(){
      // Set Command:
      this.currentCommand = this.suggestion(this.inputText, this.commandSuggestionNum);
      // Check if a Command has been Suggested (not the {} object):
      if(this.commandSelected){
        // Prepopulate with the Appropriate Number of Empty Arguments:
        this.currentArgs = this.currentCommand.params.map( () => "" );
        this.inputText = this.currentCommand.name; // Ensure consistency
      } else{
        this.currentArgs = [];
      }
    },

    // Undoes a Command Selection (returns to name entering option):
    unselectCommand(){
      this.currentCommand = {}; // Unset Command
      this.currentArgs = []; // Reset Args
      this.focusIdx = 0;
    },

    // Reset the Command Field (ex. when esc is pressed). Remove Arguments on
    // First Call. Clear InputText on Second Call.
    reset(){
      if(this.commandSelected){ // Remove Arguments on First Call
        this.unselectCommand();
        this.commandSuggestionNum = 0;
      } else{ // Clear InputText on Second Call
        this.inputText = '';
      }
    },

    // Sends the Current Command to the Log (where it is executed):
    sendCommand(){
      this.currentArgs = this.currentArgs.map( (a,idx) => {
        if(a===""){
          // Populate Any Empty Args with Default Values:
          return this.currentCommand.params[idx].defaultVal;
        } else if(a == parseFloat(a)){
          // If field contains only numbers, convert them string to numbers:
          return parseFloat(a);
        }
        return a;
      } );
      if(this.commandSelected){
        // Create name,value pair for each of the command parameters:
        let namedArgs = {};
        this.currentCommand.params.forEach(
          (param, idx) => namedArgs[param.name] = this.currentArgs[idx]
        );
        // Create new data package:
        let command = new CommandData({
          name: this.currentCommand.name,
          args: namedArgs
        });
        // Dispatch data to CLILOG to attempt to get pushed to DB:
        this.$store.dispatch('PUSH_TO_CLILOG', command);
      }
      this.currentCommand.useCount++;

      this.unselectCommand();
      this.commandSuggestionNum = 0;
    },

    // Perform Various Operations and Checks when a Key is Pressed:
    keyPressed(){
      // Fill Out Arguments:
      if(this.commandSelected){
        // Extract Entered Text by Comparing with Command Name:
        // Assumes text is a valid input (not always true, FIXME):
        this.currentArgs[this.focusIdx] += this.inputText.substring(this.currentCommand.name.length);
        this.inputText = this.currentCommand.name; // Reset reference
      }
    },

    // Advances the Input State (either to the next argument or by selecting the
    // current command):
    advanceInputState(){
      if(this.commandSelected){
        this.nextArgument();
      } else{ // No Command Yet Selected; so, select one.
        this.selectCommand();
      }
    },

    // Increment Index of Focused Argument:
    nextArgument(){
      let n = this.currentCommand.params.length;
      this.focusIdx = ((this.focusIdx + 1) % n + n) % n;
    },
    // Decrement Index of Focused Argument:
    prevArgument(){
      let n = this.currentCommand.params.length;
      this.focusIdx = ((this.focusIdx - 1) % n + n) % n;
    },

    // Returns the Nth Suggested Command for the Given Input String:
    suggestion(str, n){
      if(str=="" && n==0){ // If field is empty and scrolling hasn't occurred, suggest nothing.
        return {};
      } else{
        // Get All Commands which Match:
        let suggestions = this.commandOptions.filter( c => c.name.startsWith(str) );

        if(suggestions.length == 0){
          return {};
        } else if(n==0){
           // Don't bother with sorting, just choose command with highest useCount:
          return suggestions.reduce( (sug, val) => val.useCount > sug.useCount ? val : sug );
        } else{
          // Keep n +ve and in range:
          n = ((n % suggestions.length) + suggestions.length) % suggestions.length;
          // Sort suggestions by useCount, and return the nth entry (allows for cycling):
          return suggestions.sort( (a,b) => b.useCount - a.useCount )[n];
        } // suggestions.length?
      } // n==0?
    }, // #suggestion

    // Clear All Suggestions if Field is Empty and Backspace is Pressed:
    backspacePressed(){
      // Make Suggestions Disappear if Pressed on Empty Field:
      if(this.inputText==""){
        this.commandSuggestionNum = 0;
      }
      if(this.commandSelected){
        // Handle Backspace on Empty Argument
        if(this.currentArgs[this.focusIdx] === ""){ // == will assume 0 as a num (after submission) is empty
          if(this.focusIdx==0){ // Unselects the Current Command if Current Argument is 0 and Empty:
            this.unselectCommand();
          } else{ // Otherwise, just decrements argument focus
            this.prevArgument();
          }
        } else{
          // Process Backspace on Argument Normally:
          this.currentArgs.splice(this.focusIdx, 1, this.currentArgs[this.focusIdx].toString().slice(0,-1));
        }
      } else{
        this.inputText = this.inputText.slice(0,-1);
      }
    }
  }
}
</script>

<style lang="scss" scoped>
  @import '@/styles/_colors.scss';

  .command-field{
    background-color: $color-background;
    margin: 2rem 1rem 1rem 1rem;
    position: relative;
  }

  .command-input{
    background-color: transparent;
    color: transparent;
    width: 100%;
    height: 100%;
    resize: none;
    position: absolute;
    top: 0;
    left: 0;
    padding: 0;
    margin: 0;
    border: none;
    user-select: none;
  }
  input:focus{
    outline-width: 0; /* eliminate native highlighting */
  }

  .command-text{
    background-color: $color-grey-dark;
    color: $color-near-white;
    font-weight: normal;
    text-align: left;
    padding: 1rem .8rem;
    border-radius: .6rem;
    border: .1rem solid $color-grey-dark;
    transition : .05s linear;
    height: 4rem;
    display: flex;
    align-items: center;
  }

  .highlight{
    border: .1rem solid $color-primary;
    background-color: $color-background;
  }

  .suggested{
    color: lighten($color-near-black, 40) !important /* override color in focused if class is suggested focused */
  }

  .focused{
    background-color: $color-near-white;
    border-radius: .6rem;
    color: $color-near-black;
    padding: .5rem .9rem;
  }
</style>
