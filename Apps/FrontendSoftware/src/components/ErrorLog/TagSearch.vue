<!-- 

	**  This is the search component in the ErrorLogFilter component **
	**  It is at the top right of the ErrorLogFilter, used to select various sensors. 
	**  User can type in keywords, which will be used to populate a list of possible options
	**  from the current filter category. 

-->

<template>
  <div class="errorLogSearch">
    <div class="dropdown">
      <div class="input-button-wrapper">
        <input
          v-model="rawInput"
          ref="input"
          type="text"
          class="search__input text__main"
          @keydown="toggleDropdownOnKey()"
          @keydown.enter="enterKeyPressed()"
          @click="
            fieldFocused = true;
            showError = false;
          "
          @blur="fieldFocused = false"
          @keydown.tab.prevent="acceptSuggestion"
          @keydown.right.prevent="acceptSuggestion"
          @keydown.up.prevent="toggleDropdownHighlight('Up')"
          @keydown.down.prevent="toggleDropdownHighlight('Down')"
          @keydown.delete.prevent="deletePressed()"
          @keydown.esc.prevent="reset"
          spellcheck="false"
          placeholder="Search keyword"
        />
      </div>
      <!-- input button wrapper -->

      <!-- dropdown: showing only valid terms, user has entered something -->
      <div class="dropdown-content">
        <div
          class="dropdown-content-items-wrapper"
          v-for="(suggest, index) in allSuggestedText"
          :key="suggest"
          @mouseover="mouseOver(index)"
          @click="clickDropdownTerm()"
        >
          <p class="suggest_text">{{ protectText(suggest) }}</p>
        </div>
      </div>
    </div>
    <!-- dropdown (input + button + dropdown content) overall wrapper -->

    <!-- entered search terms -->
    <p class="text-output text-area">
      <span
        class="search__tags text__main"
        v-for="(term, index) in searchedTerms"
        :key="index"
      >
        <div class="search_tags--item_wrapper">
          <p
            class="search__tags--item"
            :class="{ highlight: fieldFocused }"
            @click="unsearchTerm(term)"
          >
            {{ protectText(term) }}
            <span class="cross_wrapper"> &#10005; </span>
          </p>
        </div>
      </span>
    </p>
  </div>
  <!-- big div -->
</template>

<script>
import TagSearch from "./TagSearch";

export default {
  name: "ErrorLogSearch",

  extends: TagSearch,

  components: {
    //TagSearch,
  },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";
@import "@/styles/_functional.scss";

/*
::placeholder {
  font-size: 1rem;
}
*/

input[type="text"] {
  font-size: 0.7rem;
}

html {
  font-size: 0.7rem;
}

.suggest_text {
  font-size: 0.7rem;
}

/* wrap the button with the input rectangle  */
.input-button-wrapper {
  background: $color-grey4;
  border: none;
  border-radius: 6px;
  padding-right: 20px;
}

input {
  background: $color-grey4;
  color: $color-near-white;
  top: 0;
  left: 0;
  padding: 11px 10px 11px 6px; /* top, right, bottom, left  */
  margin: 0;
  border: none;
  border-radius: 0px 6px 6px 0px; /* top left, top right, bot right, bot left */
  user-select: none;
}

input:focus {
  outline: $color-primary;
}

.text-area {
  color: $color-near-white;
  border: none;
  font-weight: normal;
  text-align: left;
  display: flex;
  align-items: center;
  flex-direction: row;
  flex-wrap: wrap;
}

.search_tags--item_wrapper {
  padding-right: 10px; // so the first one doesn't offset
  display: block;
}

.search__tags--item {
  padding: 8px 10px 8px 10px; // top, left, bottom, right
  //border: solid $color-grey-dark;
  border-radius: 5px;
  border-width: 2px;
  font-size: 0.5em;
  background-color: $color-primary;
}

// space between input text and the 'x' (closing icon)
.cross_wrapper {
  padding-left: 10px;
}

/* The container <div> - needed to position the dropdown content */
.dropdown {
  position: relative;
  display: inline-block;
}

/* Dropdown Content (Hidden by Default) */
.dropdown-content {
  display: none;
}

/* Dropdown Content when turned on */
.is-on {
  display: flex;
  flex-direction: row; // same width with the input block
  flex-wrap: wrap;
  position: absolute;
  min-width: 160px;
  z-index: 1;
  border-radius: 0px 0px 6px 6px; /* top left, top right, bot right, bot left */
  max-height: 150px;
  overflow-x: hidden;
  overflow-y: auto;
}

.dropdown-content-items-wrapper {
  margin-top: 0px;
  padding-left: 5px;
  overflow: hidden;
  background-color: $color-grey3;
}

.dropdown-content-items-wrapper :hover {
  background-color: $color-primary;
  width: 100%;
  height: 100%;
}

.dropdown-content-items-wrapper .highlighted {
  background-color: $color-primary;
}

.is-on .dropdown-content-items-wrapper {
  flex-basis: 100%; // each drop down item takes full width
}

.drop-down-button {
  color: white;
  cursor: pointer;
}

.unselectable {
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  -khtml-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
}
</style>
