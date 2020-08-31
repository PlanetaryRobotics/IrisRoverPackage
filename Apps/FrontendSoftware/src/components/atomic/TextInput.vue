<!--
Text Input Field that has Autocomplete and Validation Built in.
If the `validTerms` prop has been given, acceptable inputs will autocomplete as
the user types. Hitting "tab" or the "right-arrow" will accept the autocomplete.
Hitting the "up-arrow" or "down-arrow" will cycle through other autocomplete
options.

When the "enter" is hit one of two behaviours occurs:
1. If the `validTerms` prop has not been given, the `callback` will be called
with one argument consisting of all the text currently in the field. The field
will then be cleared.
2. If the `validTerms` prop has been given, the `callback` will be called with
one argument consisting of an array of the all the valid inputs which have been
entered. Note: if the user hasn't entered any valid inputs, this will be

Author: Connor Colombo, CMU
Created: 10/10/2019
Last Updated: 08/30/2020, Colombo
-->

<template>
  <div
    class="search"
    :class="{
      highlight: fieldFocused && !showError,
      error: showError
    }"
  >
    <input
      :tabindex="tabOrder"
      v-model="rawInput"
      ref="input"
      type="text"
      class="search__input text__main"
      @keydown.enter="formEvent ? $eventHub.$emit(formEvent) : searchTerm() /* If a formEvent is used to link all fields in a group, fire it; otherwise just search this field.*/"
      @click="fieldFocused = true; showError = false;"
      @focus.prevent="fieldFocused = true; showError = false;"
      @blur="fieldFocused = false"
      @keydown.tab="acceptSuggestion() /* keep default behaviour to allow normal tab cycling, just accept suggestion first*/"
      @keydown.right.prevent="acceptSuggestion"
      @keydown.up.prevent="suggestionIdx++"
      @keydown.down.prevent="suggestionIdx--"
      @keydown.delete.prevent="backspacePressed"
      @keydown.esc.prevent="reset"
      spellcheck="false"
    />
    <p class="text-output text-area">
      <span class="search__tags text__main" v-for="(term, index) in searchedTerms" :key="index">
        <p class="search__tags--item" :class="{highlight: fieldFocused}" @click="unsearchTerm(term)">
          {{ protectText(term) }}
        </p>
      </span>
      <span v-if="searchAllowed" class="text-field text-area">
        &nbsp;<span class="input-arrow" v-html="CosmosIconPack('arrowRight')"></span>
        <span class="input-text" :class="{placeholder: !rawInput && !this.suggestionIdx}">
          {{ rawInput || this.suggestionIdx ? protectText(displayInput) : placeholder }}
        </span>
        <span class="suggested">{{ protectText(suggestedText) }}</span>
      </span>
    </p>
  </div>
</template>

<script>

import CosmosIconPack from '@/styles/CosmosIconPack.js'

export default {
  name: "TextInput",
  props: {
    placeholder: { //   - Placeholder Text to be Displayed in the Field
      type: String,
      default: "",
      required: false
    },
    callback: { //      - Function to be Called when Enter is clicked on this field.
      type: Function,
      default: () => {},
      required: false
    },
    formEvent: { //     - Name of Event on Global Event Bus to Listen For. When triggered, this field will submit.
      type: String,
      default: undefined,
      required: false
    },
    errorEvent: { //    - Name of Event on Global Event Bus to Listen For. When triggered, this indicates that there is an externally-validated error in this input.
      type: String,
      default: "",
      required: false
    },
    validTerms: { //    - List of Acceptable Inputs / Autocomplete Options (OR'd with validRegex)
      type: Array,
      default: () => [],
      required: false
    },
    validRegex: { //    - Regex to Validate Acceptable Inputs / Autocomplete Options (OR'd with validTerms)
      type: RegExp,
      default: () => /^$/, // select nothing but empty strings (ie. don't play a part)
      required: false
    },
    searchLimit: { //   - Maximum Number of Terms Allowed to be Searched (if 0, callback will be called but input will not be saved)
      type: Number,
      default: Infinity,
      required: false
    },
    caseSensitive: { // - Whether the validTerms are Case Sensitive
      type: Boolean,
      default: true,
      required: false
    },
    hideText: { //      - Whether the actual written text should be hidden (eg: as *'s for password entry)
      type: Boolean,
      default: false,
      required: false
    },
    clearOnEnter: { //  - Whether the Raw Text Input Should be Cleared when Term is Submitted. (ONLY USE for searchLimit=0, #backspace behavior is undefined otherwise)
      type: Boolean,
      default: true,
      required: false
    },
    tabOrder: { //      - Relative position in the ordering of items which highlighted when cycling through them with `tab`
      type: String,
      default: "-1",
      required: false
    }
  },
  data: function() {
    return {
      CosmosIconPack,
      rawInput: "", //      - Raw, Unfiltered, Unprocessed, Unsubmitted User Input
      showError: false, //  - Whether to Display Error
      searchedTerms: [], // - All Searched Terms. Validated if Applicable
      suggestionIdx: 0, //  - Index which Allows for Cycling Through Suggestions
      fieldFocused: false //- Whether the User's Focus is Actively on the Input
    }
  },
  created: function() { // Adds event listeners to the global event hub
    // Wait for App.vue window to acknowledge that the window is activated and
    // visible before animating any content:
    this.$eventHub.$on(this.formEvent, this.searchTerm);
    this.$eventHub.$on(this.errorEvent, this.showErrorOnEvent);
  },
  beforeDestroy: function() { // Removes event listners from the global event hub
    this.$eventHub.$off(this.formEvent, this.searchTerm);
    this.$eventHub.$off(this.errorEvent, this.showErrorOnEvent);
  },
  computed: {
    // Whether a Search / Submission of the Current Input Text is Allowed
    // (searching is always allowed if limit is 0 but the output won't be saved.)
    searchAllowed(){
      return !this.searchLimit || this.searchedTerms.length < this.searchLimit;
    },
    // Returns the Remainder of the Suggested Command (not currently input by
    // the user).
    suggestedText(){
      let sug = this.suggestion(this.rawInput, this.suggestionIdx);
      if(!this.hideText){ // Replace Spaces with nbsp if they'll show up:
        sug = sug.replace(/ /g, '\u00a0');
      }
      return sug.substring(this.rawInput.length);
    },
    // Aesthetically Processed Version of rawInput for HTML display:
    displayInput(){
      let txt = this.rawInput;
      if(!this.hideText){ // Replace Spaces with nbsp if they'll show up:
        txt = txt.replace(/ /g, '\u00a0');
      }
      return txt;
    }
  },
  methods: {
    reset: function(){ // Reset Input Field to Default State:
    // Essentially a shorthand for manually removing all searched terms.
      this.rawInput = ""; // Clear Input
      this.searchedTerms.splice(0, this.searchedTerms.length); // Reactively empty array
      this.callback(this.searchedTerms); // Alert the Callback of the now empty field.
    },

    unsearchTerm: function(term) { // Remove Term from List:
      let idx = this.searchedTerms.indexOf(term);
      if(idx > -1){
        this.searchedTerms.splice(idx,1);
      }
    },
    /* Saves a Search Term from the Raw Input if It's in the List of Valid Terms
    (if one was give). Sends the New List of Valid Terms to the Callback.
    Special Case:
    Maximum Number of Terms Allowed to be Searched is `searchLimit`. If it's 0,
    callback will be called but input will not be saved to `searchedTerms`.
    */
    searchTerm: function() {
      let success = false; // Whether a valid term was found and selected during search
      
      if (this.rawInput != "") { // If text has been entered in the raw input field
        if(this.searchAllowed || this.searchLimit == 0){
         // If Searching is Still Allowed (or no items have been searched <-special case):
          let valid = true; // Default for no validation.
          if(this.validTerms.length){ // If Terms are Given Validate Against
            if(this.caseSensitive){ // Case-Sensitive Check:
              valid = this.validTerms.includes(this.rawInput);
            } else{ // Case-Insensitive Check:
              let idx = this.validTerms.map(s => s.toLowerCase()).indexOf(this.rawInput.toLowerCase());
              if(idx > -1){ // valid
                // Replace Search Text with Correct Capitalization:
                this.rawInput = this.validTerms[idx];
              } else{
                valid = false;
              }
            }
          }
          valid |= this.validRegex.test(this.rawInput);

          if(valid && !this.searchedTerms.includes(this.rawInput)){
            /* If valid and not already in list, add a bubble with the term.
            (shouldn't do this `includes` test earlier since rawInput Capitalization
            might not be correct before processing) */
            success = true;
          }
        }
      }

      if(success){
        if(this.searchLimit == 0){
          this.callback([this.rawInput]); // Call the callback but don't save.
        } else if(this.searchAllowed){
          this.searchedTerms.push(this.rawInput);
          this.callback(this.searchedTerms); // Call the callback.
        }
      } else{
        this.showErrorOnEvent();
      }

      if(this.clearOnEnter){
        this.rawInput = ""; // Always clear the raw input field on search.
      }
    },

    // Accepts the Currently Displayed Input Suggestion (if there is one).
    acceptSuggestion(){
      let sug = this.suggestion(this.rawInput, this.suggestionIdx);
      if(sug){
        this.rawInput = sug;
      }
    },
    // Returns the Nth Suggested Term for the Given Input String:
    suggestion(str, n){
      // If no valid terms given, suggest nothing.
      // If field is empty and scrolling hasn't occurred, suggest nothing.
      if(str=="" && n==0 || !this.validTerms.length){
        return "";
      } else{
        // Get All Commands which Match:
        let suggestions = this.validTerms.filter( t => {
          if(this.caseSensitive){
            return t.startsWith(str);
          } else{
            return t.toLowerCase().startsWith(str.toLowerCase());
          }
        });

        if(suggestions.length){ // If there's anything to suggest:
          // Keep n +ve and in range:
          n = ((n % suggestions.length) + suggestions.length) % suggestions.length;
          return suggestions[n];
        } else{
          return "";
        }
      }
    }, // #suggestion

    // Special Handling for When Backspace is Pressed:
    backspacePressed(){
      // Process Backspace Normally:
      if(this.rawInput){
        this.rawInput = this.rawInput.slice(0, -1); // Remove last character.
      } else{ // If pressed on a field that started empty:
        this.searchedTerms.splice(-1,1);
      }

      // Make Suggestions Disappear if Pressed on or Results in Empty Field:
      if(!this.rawInput){
        this.suggestionIdx = 0;
      }
    },

    /* Protects the Given Text by Returning a Version of the Same Number of
    Characters but with Each Character Hidden with *'s if `hideText` is true. */
    protectText(txt){
      return this.hideText ? "●".repeat(txt.length) : txt;
    },
    
    // Makes an error visible (needs to be named for event registration/deregistration):
    showErrorOnEvent(){
      this.showError = true;
    }
  }
}
</script>

<style lang="scss" scoped>
  @import '@/styles/_colors.scss';
  @import '@/styles/_functional.scss';

  .search {
    user-select: none;
    position: relative;
    display: flex;
    background-color: $color-grey-dark;
    border-radius: 1rem;
    overflow: hidden;
    padding: .5rem .8rem;
    border: .1rem solid $color-grey-dark;
    transition : .05s linear;

    &:focus {
      outline-width: 0; /* eliminate native highlighting */
      outline: none;
    }

    &.highlight{
      border-color: $color-primary;
      background-color: $color-background;
    }

    &.error{
      border-color: $color-danger;
      background-color: $color-background;
    }

    &__tags{
      z-index: 2;
      &--item {
        padding: .3rem .8rem;
        border-radius: .5rem;
        background-color: $color-background;
        margin-right: .5rem;
        user-select: none;
        cursor: pointer;
        transition: .1s ease-in-out;

        &.highlight{
          background-color: $color-grey-dark;
        }

        &:hover {
          background-color: $color-primary !important;
        }
      }
    }

    &__input {
      z-index: 1;
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

      &:focus {
        outline-width: 0; /* eliminate native highlighting */
        outline: none;
      }
    }
  }

  .suggested{
    color: lighten($color-near-black, 40) !important /* override color in focused if class is suggested focused */
  }

  .placeholder {
    color: $color-grey;
  }
  
  .text-area{
    color: $color-near-white;
    border: none;
    font-weight: normal;
    text-align: left;
    display: flex;
    align-items: center;
  }

  .text-output{
    $horiz-padding: 1rem;

    padding: $horiz-padding .8rem;
    border-radius: .6rem;
    height: 4rem; 

    & > .text-field {
      display: flex;
      align-items: center;
      line-height: 0;

      & > .input-arrow{
        width: 12px;
        height: 12px;
        margin-right: $horiz-padding;
        line-height: 0;
        stroke: $color-near-white;
      }
    }
  }

</style>
