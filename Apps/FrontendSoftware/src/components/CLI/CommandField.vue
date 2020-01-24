<!--
Field for a Command to be Entered, which includes autocomplete functionality
which adapts to a user's most used commands.

TODO:
  - Limit number of commands which can be in waiting (THIS SHOULD BE 1 OTHERWISE ORDER IS NOT GUARANTEED)
  - User permission checking

Author: Connor W. Colombo, CMU
Created: 1/31/2019
Last Updated: 11/8/2019, Colombo
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
        <transition name="fade" mode="out-in">
          <span key="1" v-if="!inputWarning[idx]" v-bind:class="{complete: arg!=='', suggested: arg==='', focused: idx == focusIdx}">
            {{ arg=="" ? currentCommand.params[idx].name : arg }}<span v-if="arg" v-html="currentCommand.formattedUnits[idx]"></span>
          </span>
          <span key="2" v-if="inputWarning[idx]" class="focused--warning">
            <span v-if="arg" v-html="inputWarning[idx]"></span>
          </span>
        </transition>
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
      inputWarning: [], //        - Warning Info about Invalid Input for Given Argument
      inputWarningTimer: [] //    - Timers to Clear Input Warnings
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
      this.showArgument = this.currentCommand.params.map( () => 1 );
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
        this.clearInputWarning(); // Immediately clear any existing input warnings
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
        } else{
          return a;
        }
      } );

      if(this.commandSelected){
        let valid = true;
        for(let i in this.currentArgs){
          valid &= this.validateArgument(i);
        }
        if(valid){
          // Created Named Args List:
          let namedArgs = {Images: 1};
          for(let i in this.currentCommand.params){
            namedArgs[this.currentCommand.params[i].name] = this.currentArgs[i];
          }
          // Construct Command Data Object:
          let command = new CommandData({
            name: this.currentCommand.name,
            args: namedArgs
          });
          // Push Data to Log and DB:
          this.$store.dispatch('PUSH_TO_CLILOG', command);
          this.currentCommand.useCount++;

          this.unselectCommand();
          this.commandSuggestionNum = 0;
        }
      }
    },

    // Determines whether the given input is valid for the given type.
    // If 'specialsAllowed', inputs that contain special characters permitted
    // for the input's type which would normally fail as written will not fail.
    // (eg. "1e" and  "-" are not valid floats but are necessary interim states
    // to be able to write the numbers "1e6" and "-1").
    // Returns an object with the following elements:
    // - valid: whether the input is valid
    // - outOfBounds: whether the input is invalid because of an out of bounds
    //   error.
    // - warningString: String of text for use in a warning that indicates this
    //   input as invalid
    validateInput(input, type, specialsAllowed){
      // Input Validation (one character at a time while still considering the complete result):
      let regexCharCheck = {
        "string": /.*/, // Any character
        "float": /^[-\d.e]*$/i, // only digits, minus sign, decimal, or "e"
        "uint8": /^[\d]*$/, // only digits
        "int8":  /^[-\d]*$/ // only digits or minus sign
      }
      // Allowed Special Character States (parseX not pass in these states):
      // (if the string was evaluated with these characters in these sequences
      // right after they were entered, the parser test would fail,
      // eg. "1e" or just "-" fails):
      let regexSpecialCharCheck = {
        "float": /\d[e]$|^[-]$/i, // string must be only "-", or end with a digit followed by an "e"
        "int8": /^[-]$/  // string must be only "-"
      }
      let typeChecks = {
        "string": x=>x,
        "float": parseFloat,
        "uint8": parseInt,
        "int8": parseInt
      }
      let limits = {
        "float": [-1e9,1e9],
        "uint8": [0,255],
        "int8":  [-128,127]
      }
      let valid = false;
      let outOfBounds = false;
      let warningString = "";
      // If the new character is a member of this type's set:
      if(input === ""){
        valid = true; // ensure this trivial base case is considered valid
      } else if(regexCharCheck[type].test(input)){
        // Ensure that the new argument formed by adding it will still be a valid member of the type
        // (can't go straight to this step b/c "." would make it through for int types):
        let parsedValue = typeChecks[type](input);
        if(parsedValue == input){
          // Ensure the parsed value is within acceptable limits, if there are any:
          if(parsedValue < limits[type][0]){
            warningString = input + " < " + limits[type][0]; // Pop up a warning
            outOfBounds = true;
          } else if(parsedValue > limits[type][1]){
            warningString = input + " > " + limits[type][1]; // Pop up a warning
            outOfBounds = true;
          } else{
            valid = true;
          }
        } else if(specialsAllowed && type in regexSpecialCharCheck && regexSpecialCharCheck[type].test(input)){
          // If string fails to parse but contains a valid special character:
          valid = true; // if specials are allowed, this is fine
        }
      }
      if(!valid && !warningString){ // Default warning string.
        warningString = input + " is invalid";
      }
      return {valid, outOfBounds, warningString};
    },

    // Displays the given warning in the slot for the argument with index
    // 'argIdx'. This warning will disapppear after 'timeout' ms (if not given,
    // default is 2000ms). If an index is not given, the warning will be
    // displayed for the active argument.
    warnArgument(warning, argIdx, timeout=2000){
      argIdx = argIdx || this.focusIdx; // default value
      // Display Warning:
      this.inputWarning.splice(argIdx, 1, warning);

      // Clear warning after a bit:
      clearTimeout(this.inputWarningTimer[argIdx]); // Clear existing timer
      this.inputWarningTimer[argIdx] = setTimeout(() => {
        this.clearInputWarning(argIdx);
      }, timeout);
    },

    // Checks the to make sure given argument with the given argument is
    // completely valid, if it's not, a warning is generated.
    // Returns whether this argument was valid.
    validateArgument(argIdx){
      let {valid, warningString} = this.validateInput(this.currentArgs[argIdx], this.currentCommand.params[argIdx].type, false);

      if(!valid){ // Warn if a warning was given:
        this.warnArgument(warningString, argIdx);
      }

      return valid;
    },

    // Dismisses the input warning for the argument with the given index
    // 'argIdx'. If an index is not given, the warning for the active argument
    // will be cleared.
    clearInputWarning(argIdx){
      argIdx = argIdx || this.focusIdx; // default value
      this.inputWarning.splice(argIdx, 1, ""); // reset array value reactively
    },

    // Whether there is an input warning currently active
    inputWarningPresent(){
      return this.inputWarning[this.focusIdx] != "";
    },

    // Perform Various Operations and Checks when a Key is Pressed:
    keyPressed(){
      // Fill Out Arguments:
      if(this.commandSelected){
        if(this.inputText[this.inputText.length-1] == ','){
          // Alternative way to advance state is to follow the displayed syntax.
          this.inputText = this.inputText.slice(0,-1);
          this.nextArgument();
        } else{
          this.clearInputWarning(); // Immediately clear any existing input warnings
          let input = this.inputText.substring(this.currentCommand.name.length); // Extract Entered Text by Comparing with Command Name
          let type = this.currentCommand.params[this.focusIdx].type;
          // Keep adding characters until input is invalid:
          for(let char of input){
            let newArg = this.currentArgs[this.focusIdx] + char;
            let {valid, outOfBounds, warningString} = this.validateInput(newArg, type, true);

            if(valid){ // Accept new character if valid:
              this.currentArgs.splice(this.focusIdx, 1, newArg);
            } else if(outOfBounds){ // Only show outOfBounds warnings here. Otherwise, just refuse the input.
              this.warnArgument(warningString + this.currentCommand.formattedUnits[this.focusIdx]);
            }
          }  // for char of input
        }
        this.inputText = this.currentCommand.name; // Reset reference
      } else if(this.inputText[this.inputText.length-1] == '['){
        // Alternative way to select a command is to follow the displayed syntax.
        this.inputText = this.inputText.slice(0,-1);
        this.selectCommand();
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
      this.changeArgument(1);
    },
    // Decrement Index of Focused Argument:
    prevArgument(){
      this.changeArgument(-1);
    },
    // Changes the Argument Pointer in the Direction Given by the Sign of dir
    changeArgument(dir){
      if(this.validateArgument(this.focusIdx)){
        // Only change the focus if this argument is valid. If it's not,
        // #validateArgument generates a warning.
        this.clearInputWarning(); // Immediately clear any existing input warnings
        let n = this.currentCommand.params.length;
        this.focusIdx = ((this.focusIdx + dir) % n + n) % n;
      }
    },

    // Returns the Nth Suggested Command for the Given Input String:
    suggestion(str, n){
      if(str=="" && n==0){ // If field is empty and scrolling hasn't occurred, suggest nothing.
        return {};
      } else{
        // Get All Commands which Match (and correct incorrect capitalization upon selection):
        let suggestions = this.commandOptions.filter( c => c.name.toLowerCase().startsWith(str.toLowerCase()) );

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
      } // str==0 && n==0?
    }, // #suggestion

    // Clear All Suggestions if Field is Empty and Backspace is Pressed:
    backspacePressed(){
      if(this.inputWarningPresent()){
      // Just dismiss the active input warning if there is one.
        this.clearInputWarning(); // Immediately clear any existing input warnings
      } else{
      // Handle backspace normally
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

    &--warning{
      @extend .focused;
      background-color: $color-caution !important;
    }
  }
  .fade-enter-active, .fade-leave-active {
    transition: opacity 0.0s
  }
  .fade-enter, .fade-leave-to {
    opacity: 0
  }
</style>
