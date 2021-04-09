<template>
  <div class="tag-search">
    <div class="dropdown">
      <!-- Extends TextInput -->

      <div class="input-button-wrapper">
        <input
          ref="input"
          v-model="rawInput"
          type="text"
          class="search__input text__main"
          spellcheck="false"
          placeholder="Search"
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
        >

        <!-- manually open & close dropdown UI -->

        <span
          class="drop-down-button unselectable"
          @mousedown="toggleDropdown()"
        >
          {{ current_icon.valueOf() }}
        </span>
      </div>
      <!-- input (input field + button) wrapper -->

      <!-- dropdown: showing only valid terms, user has entered something -->

      <div
        v-if="hasSuggestions()"
        class="dropdown-content"
      >
        <div
          v-for="(suggest, index) in allSuggestedText"
          :key="suggest"
          class="dropdown-content-items-wrapper"
          @mouseover="mouseOver(index)"
          @click="clickDropdownTerm()"
        >
          <p>{{ protectText(suggest) }}</p>
        </div>
      </div>

      <!-- dropdown: showing all possible valid terms, user has not entered anything -->

      <div
        v-else
        class="dropdown-content"
      >
        <div
          v-for="(option, index) in validTerms"
          :key="option"
          class="dropdown-content-items-wrapper"
          @mouseover="mouseOver(index)"
          @click="clickDropdownTerm()"
        >
          <p>{{ protectText(option) }}</p>
        </div>
      </div>
    </div>
    <!-- dropdown (input + button + dropdown content) overall wrapper -->

    <!-- entered search terms -->

    <p class="text-output text-area">
      <span
        v-for="(term, index) in searchedTerms"
        :key="index"
        class="search__tags text__main"
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
  <!-- overall container -->
</template>

<script>
import TextInput from '../atomic/TextInput';
import $ from 'jquery';
import colors from '@/styles/_colors.scss';
export default {
    name: 'TagSearch', //  - Free to use methods from the TextInput component

    components: {
    // TextInput,
    },

    extends: TextInput,

    props: {
        validTerms: {
            //   - Given list of possible options to be displayed under the text box.
            type: Array,
            default: () => [],
            required: false,
        },
    },

    data: function () {
        return {
            dropdownIsOpen: false,
            // icons might later switch to svgs
            current_icon: '\u{02228}',
            ui_icons: { closed: '\u{02228}', open: '\u{0002B}' },
            suggestionIdx: -1,
        };
    },

    computed: {
    // Returns the Remainder of the Suggested Command (not currently input by
    // the user).
        suggestedText() {
            return this.fullSuggestedText;
        },

        // Returns the Full Text of the Suggested Command.
        fullSuggestedText() {
            let sug = this.suggestion(this.rawInput, this.suggestionIdx);
            return this.makeHTMLReadable(sug);
        },

        // Returns an Array of the Readable Full Text of all Suggested Commands.
        allSuggestedText() {
            return this.allSuggestions(this.rawInput).map((sug) =>
                this.makeHTMLReadable(sug)
            );
        },
    },

    methods: {
    // Functionality functions for filtering through valid terms
    // Helper function that makes the given suggestion string readable (through HTML)

        makeHTMLReadable(str) {
            if (!this.hideText) {
                // Replace Spaces with nbsp if they'll show up:
                return str.replace(/ /g, '\u00a0');
            } else {
                return str;
            }
        },

        // Returns an Array of All Suggested Terms for the Given Input String:

        allSuggestions(str) {
            if (str == '' || !this.validTerms.length) {
                return [];
            } else {
                // Get All Commands which Match:
                let suggestions = this.validTerms.filter((t) => {
                    if (this.caseSensitive) {
                        return t.startsWith(str);
                    } else {
                        return t.toLowerCase().startsWith(str.toLowerCase());
                    }
                });
                return suggestions;
            }
        },

        // Returns the Nth Suggested Term for the Given Input String:

        suggestion(str, n) {
            // If no valid terms given, suggest nothing.
            // If field is empty and scrolling hasn't occurred, suggest nothing.
            if ((str == '' && n == 0) || !this.validTerms.length) {
                return '';
            } else {
                // Get All Commands which Match:

                let suggestions = this.allSuggestions(str);
                if (suggestions.length) {
                    // If there's anything to suggest:
                    // Keep n +ve and in range:
                    n =
            ((n % suggestions.length) + suggestions.length) %
            suggestions.length;
                    return suggestions[n];
                } else {
                    return '';
                }
            }
        }, // #suggestion

        // helper function that checks if the index is valid, returns the given index
        // if valid, and returns the closest index in bound if not

        checkValidIndex(Idx) {
            // check if we are displaying ALL valid terms or only suggested terms
            // this influences the upper bound of the index

            let sugLen = this.allSuggestedText.length;
            let currentLength = sugLen == 0 ? this.validTerms.length : sugLen;

            // check if out of bounds
            if (Idx == currentLength) {
                return Idx - 1;
            } else if (Idx < 0) {
                return 0;
            } else {
                return Idx;
            }
        },

        ///////////////////////////////////

        // UI Function

        // loop through all dropdown and clear all highlights

        clearHighlight() {
            let elements = document.getElementsByClassName(
                'dropdown-content-items-wrapper'
            );
            let elementsL = [...elements];
            elementsL.map(function (element) {
                element.style.background = colors.colorgrey3;
            });
        }, 

        mouseOver(index) {
            this.clearHighlight();
            let elements = document.getElementsByClassName(
                'dropdown-content-items-wrapper'
            );
            this.suggestionIdx = index;
            let curHighlighted = elements[index];
            curHighlighted.style.background = colors.colorprimary;
        },

        deletePressed() {
            this.backspacePressed();

            // before deleting accepted terms
            if (this.rawInput == '' && this.dropdownIsOpen) {
                this.turnDropdownOff();
            }

            // do not want to set suggestionIdx to 0, need -1 to highlight correctly
            this.suggestionIdx = -1;
            this.clearHighlight();
        },

        //dropdown highlight

        toggleDropdownHighlight(direction) {
            this.clearHighlight();
            let elements = document.getElementsByClassName(
                'dropdown-content-items-wrapper'
            );
            let currentIdx = this.suggestionIdx;

            // avoid invalid index into elements

            if (elements.length != 0) {
                if (direction == 'Down' && currentIdx == -1) {
                    let curHighlighted = elements[0];
                    curHighlighted.style.background = colors.colorprimary;
                    this.suggestionIdx = 0;
                } else if (direction == 'Up' && currentIdx == -1) {
                    console.log('Cannot up');
                } else if (direction == 'Up') {
                    let curHighlighted = elements[this.checkValidIndex(currentIdx)];
                    let nextIdx = this.checkValidIndex(currentIdx - 1);
                    let nextHighlighted = elements[nextIdx];
                    // changing css to switch highlight
                    curHighlighted.style.background = colors.colorgrey3;
                    nextHighlighted.style.background = colors.colorprimary;
                    // update suggestion index
                    this.suggestionIdx = nextIdx;
                    // scroll animation if needed

                    if (nextIdx < 3) {
                        this.scrollDropdown('Up');
                    }
                } else {
                    let curHighlighted = elements[this.checkValidIndex(currentIdx)];
                    let nextIdx = this.checkValidIndex(currentIdx + 1);
                    let nextHighlighted = elements[nextIdx];
                    // changing css to switch highlight
                    this.clearHighlight();
                    curHighlighted.style.background = colors.colorgrey3;
                    nextHighlighted.style.background = colors.colorprimary;
                    // update index
                    this.suggestionIdx = nextIdx;
                    // scroll animation if needed
                    if (nextIdx >= 3) {
                        this.scrollDropdown('Down');
                    }
                }
            }
        },

        // dropdown content switch

        hasSuggestions() {
            if (this.rawInput == '') {
                return false;
            }

            // rawInput != "", user has entered

            return true;
        },

        // switch ui + turn on dropdown

        turnDropdownOn() {
            this.dropdownIsOpen = true;
            this.current_icon = this.ui_icons.open;
            $('.dropdown-content').addClass('is-on');
        },

        // switch ui + turn off dropdown

        turnDropdownOff() {
            this.dropdownIsOpen = false;
            this.current_icon = this.ui_icons.closed;
            $('.dropdown-content').removeClass('is-on');
            this.clearHighlight();
            this.suggestionIdx = -1;
        },

        // only turn it on, avoid glitching

        toggleDropdownOnKey() {
            if (this.dropdownIsOpen == false) {
                this.turnDropdownOn();
            }
        },

        // accept the term that is highlighted that is selected by toggling up down key or mouse click

        acceptOnEnterWithKeyMouse() {
            let tmp = this.rawInput;

            // check if we are displaying validTerms or suggestions(sublist of validTerms) so we index correctly

            this.rawInput =
        this.allSuggestedText.length == 0
            ? this.validTerms[this.suggestionIdx]
            : this.allSuggestedText[this.suggestionIdx];

            this.searchTerm();

            this.rawInput = tmp;
        },

        // after 'enter' is pressed, call search term and turn the dropdown off

        enterKeyPressed() {
            this.acceptOnEnterWithKeyMouse();
            this.searchTerm();
            this.turnDropdownOff();
        },

        // user clicks on dropdown terms, accept and turn off the dropdown

        clickDropdownTerm() {
            this.acceptOnEnterWithKeyMouse();
            this.turnDropdownOff();
        },

        // called when manually clicking the dropdown ui

        toggleDropdown() {
            if (this.dropdownIsOpen) {
                this.turnDropdownOff();
            } else {
                this.turnDropdownOn();
            }
        },

        // dropdown automatic scroll up/down animation

        scrollDropdown(direction) {
            if (direction == 'Up') {
                $('.dropdown-content').stop().animate(
                    {
                        scrollTop: 0,
                    },
                    800
                );
            } else {
                $('.dropdown-content')
                    .stop()
                    .animate(
                        {
                            scrollTop: $('.dropdown-content')[0].scrollHeight,
                        },
                        800
                    );
            }
        },
    },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";

@import "@/styles/_functional.scss";
// wrap the button with the input rectangle

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
  padding: 8px 20px 8px 20px; // top, left, bottom, right
  margin: 0;
  border: none;
  border-radius: 6px;
  user-select: none;
}

input:focus {
  outline: none;
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
  padding: 8px 20px 8px 20px; // top, left, bottom, right
  border: solid $color-grey-dark;
  border-radius: 72px;
  border-width: 2px;
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
  min-width: 305px;
  z-index: 1;
  border-radius: 6px;
  max-height: 150px;
  overflow-x: hidden;
  overflow-y: auto;
}

.dropdown-content-items-wrapper {
  margin-top: 0px;
  padding-left: 20px;
  overflow: hidden;
  background: $color-grey3; // #666666 in FIGMA
}

.dropdown-content-items-wrapper .highlighted {
  background: $color-primary;
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

'co
