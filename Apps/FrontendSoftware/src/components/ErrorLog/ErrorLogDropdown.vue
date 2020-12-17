<!-- 

	**  This is the dropdown component in the ErrorLogFilter component **
	**  It is at the top left of the ErrorLogFilter, used to toggle between filter categories. 
	**  Categories: Error Tag, Error ID, Sensory Name, Mission Time Frame,
	**              Created By, Date Created, Date Last Mdoified, Category **

-->

<template>
  <div class="searchBar">
    <div class="dropdown">
      <div class="dropdown-header-wrapper">
        <span class="dropdown-header">
          <span class="medium"> Search </span> {{ this.currentCategory }}
        </span>

        <span
          class="drop-down-button unselectable"
          @mousedown="toggleDropdown()"
        >
          {{ this.current_icon.valueOf() }}
        </span>
      </div>
      <!-- dropdown header wrapper -->

      <div class="dropdown-content">
        <div
          class="dropdown-content-items-wrapper"
          v-for="(category, index) in searchCategories"
          :key="category"
          @click="clickDropdownTerm(index)"
        >
          <p>{{ category }}</p>
        </div>
        <!--dropdown content wrapper -->
      </div>
      <!-- dropdown content -->
    </div>
    <!-- dropdown -->
  </div>
  <!-- big container -->
</template>

<script>
import $ from "jquery";
import colors from "@/styles/_colors.scss";

export default {
  name: "ErrorLogDropdown",

  data: function () {
    return {
      searchCategories: [
        "Error Tag",
        "Error ID",
        "Sensory Name",
        "Mission Time Frame (M+)",
        "Created By",
        "Date Created",
        "Date Last Modified",
        "Category",
      ],
      categoryIdx: 0,
      dropdownIsOpen: false,
      // icons might later switch to svgs
      current_icon: "\u{02228}",
      ui_icons: { closed: "\u{02228}", open: "\u{0002B}" },
      suggestionIdx: -1,
      currentCategory: "Error Tag",
    };
  },

  methods: {
    // called when manually clicking the dropdown ui
    toggleDropdown() {
      if (this.dropdownIsOpen) {
        this.turnDropdownOff();
      } else {
        this.turnDropdownOn();
      }
      console.log("dropdown toggled");
    },

    // switch ui + turn on dropdown
    turnDropdownOn() {
      this.dropdownIsOpen = true;
      this.current_icon = this.ui_icons.open;
      $(".dropdown-content").addClass("sensor-is-on");
    },

    // switch ui + turn off dropdown
    turnDropdownOff() {
      this.dropdownIsOpen = false;
      this.current_icon = this.ui_icons.closed;
      $(".dropdown-content").removeClass("sensor-is-on");
      this.clearHighlight();
      this.suggestionIdx = -1;
    },

    mouseOver(index) {
      this.clearHighlight();
      let elements = document.getElementsByClassName(
        "dropdown-content-items-wrapper"
      );
      this.suggestionIdx = index;
      this.categoryIdx = index;
      let curHighlighted = elements[index];
      curHighlighted.style.background = colors.colorprimary;
      //console.log("updated idx to " + this.categoryIdx );
    },

    clickDropdownTerm(index) {
      this.index = index;
      this.categoryIdx = index;
      this.currentCategory = this.searchCategories[this.categoryIdx];
      console.log(
        "clicked" +
          this.categoryIdx +
          "the category is: " +
          this.searchCategories[this.categoryIdx]
      );
      this.$emit(
        "Update:SearchCategory",
        this.searchCategories[this.categoryIdx]
      );
    },

    // loop through all dropdown and clear all highlights
    clearHighlight() {
      let elements = document.getElementsByClassName(
        "dropdown-content-items-wrapper"
      );
      let elementsL = [...elements];
      elementsL.map(function (element) {
        element.style.background = colors.colorgrey3;
      });
    },

    //dropdown highlight
    toggleDropdownHighlight(direction) {
      this.clearHighlight();
      let elements = document.getElementsByClassName(
        "dropdown-content-items-wrapper"
      );
      let currentIdx = this.suggestionIdx;

      // avoid invalid index into elements
      if (elements.length != 0) {
        if (direction == "Down" && currentIdx == -1) {
          let curHighlighted = elements[0];
          curHighlighted.style.background = colors.colorprimary;
          this.suggestionIdx = 0;
        } else if (direction == "Up" && currentIdx == -1) {
          console.log("Cannot up");
        } else if (direction == "Up") {
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
            this.scrollDropdown("Up");
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
            this.scrollDropdown("Down");
          }
        }
      }
    },

    // dropdown automatic scroll up/down animation
    scrollDropdown(direction) {
      if (direction == "Up") {
        $(".dropdown-content").stop().animate(
          {
            scrollTop: 0,
          },
          800
        );
      } else {
        $(".dropdown-content")
          .stop()
          .animate(
            {
              scrollTop: $(".dropdown-content")[0].scrollHeight,
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

* {
  font-size: 0.7rem;
}

span {
  color: $color-near-white;
}

span .medium {
  font-weight: 400;
}

/* The container <div> - needed to position the dropdown content */
.dropdown {
  position: relative;
  display: inline-block;
  float: left;
}

/* Dropdown Content (Hidden by Default) */
.dropdown-content {
  display: none;
}

/* Dropdown Content when turned on */
.sensor-is-on {
  display: flex;
  flex-direction: row; // same width with the input block
  flex-wrap: wrap;
  position: absolute;
  min-width: 130px;
  z-index: 1;
  border-radius: 0px 0px 6px 6px;
  max-height: 129px;
  overflow-x: hidden;
  overflow-y: auto;
}

.dropdown-header-wrapper {
  background: $color-grey3; // #666666 in FIGMA
  padding: 10px;
  padding-left: 12px;
  padding-right: 12px;
  border-radius: 6px 0 0 6px;
}

.dropdown-content-items-wrapper {
  margin-top: 0px;
  padding-left: 10px;
  overflow: hidden;
  background: $color-grey3; // #666666 in FIGMA
}
.dropdown-content-items-wrapper:hover {
  background-color: #8a6dff;
}

.dropdown-content-items-wrapper .highlighted {
  background: $color-primary;
}

.sensor-is-on .dropdown-content-items-wrapper {
  flex-basis: 100%; // each drop down item takes full width
}

.drop-down-button {
  color: white;
  cursor: pointer;
  padding-left: 10px;
}

.unselectable {
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  -khtml-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
}

input {
  background: $color-grey4;
  color: $color-near-white;
  top: 0;
  left: 0;
  padding: 10px 20px 10px 20px; // top, left, bottom, right
  margin: 0;
  border: none;
  border-radius: 0px 6px 6px 0px;
  user-select: none;
  font-size: 16px;
  font-weight: 300;
}

input:focus {
  outline: none;
  box-shadow: 0 0 0 1pt $color-primary;
}
</style>
