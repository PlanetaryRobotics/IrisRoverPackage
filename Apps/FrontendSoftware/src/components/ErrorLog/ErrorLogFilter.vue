<template>
  <div class="error-log-filter">
    <div class="centered_content">
      <!--------------------- TOP SECTION ------------------------->
      <div class="header">
        <p class="header_title">Errors Log</p>

        <div class="header_icons">
          <div class="icon">
            <span
              class="sort_icon"
              v-html="CosmosIconPack('sort')"
              @click="
                let original = sortPopUpOpen;
                sortPopUpOpen = !original;
              "
            >
            </span>
          </div>

          <div class="icon">
            <span
              class="new_window_icon"
              v-html="CosmosIconPack('newWindow')"
            ></span>
          </div>
        </div>
      </div>

      <!-- **********************************
            TODO:  Sort PopUp Window 
            - Anchored by the UI sort symbol 
            - on top of search 
           **********************************
      -->
      <div class="sort_popup popup" v-if="sortPopUpOpen">
        <PopUpDropdown
          class="sort_popup_content"
          :options="sortOptions"
          :title="sortTitle"
          v-on:update:sort:input="
            sortCard($event);
            sortPopUpOpen = false;
          "
        />
      </div>

      <div class="relative__parent">
        <div class="top_bar fixed__Child">
          <ErrorLogDropdown
            v-on:update:search-category="searchCategoryUpdate($event)"
          />

          <!-- **********************************
            TODO:  ErroLogSearch
            - Key down not working (click works)
            - Dropdown hover color background not full 
           **********************************
      -->
          <div>
            <ErrorLogSearch
              :validTerms="validTerms"
              v-on:update:search-input="filterCardBySearch($event)"
            />
          </div>
        </div>
      </div>

      <!---------------------  MAIN SCREEN ------------------------->

      <div class="priority_accordion_container" v-if="State == 'Priority'">
        <!-- Sort Priority from Low to High -->
        <div v-if="this.sortBy == 'LowToHigh'">
          <div class="row">
            <p class="sort_type_text">Sorted by: Low - High Priority</p>

            <!-- **********************************
                TODO: Expand All
                - doesn't work some time 
                - try to use $emit instead of DOM click 
            -->
            <span
              class="expand_text unselectable"
              @click="
                expandAllAccordion();
                $emit('expandall:fromparent');
              "
            >
              Expand All
            </span>
          </div>

          <div v-for="priority in this.priorities" v-bind:key="priority">
            <hr class="accordion_lines" />
            <AccordionWithHeader
              class="accordion_with_header"
              :header="priority"
              :fixed_header="fixed_header_priority"
              :contents="
                checkFilter(errorTags).filter(
                  (errorTag) => errorTag.priority == priority
                )
              "
            />
          </div>
        </div>

        <!-- Sort Priority from High to Low -->
        <div v-else>
          <p class="sort_type_text">Sorted by: High - Low Priority</p>
          <div
            v-for="priority in this.priorities.reverse()"
            v-bind:key="priority"
          >
            <hr class="accordion_lines" />
            <AccordionWithHeader
              class="accordion_with_header"
              :header="priority"
              :fixed_header="fixed_header_priority"
              :contents="
                checkFilter(errorTags).filter(
                  (errorTag) => errorTag.priority == priority
                )
              "
            />
          </div>
        </div>

        <!-- Last Line -->
        <hr class="accordion_lines" />
      </div>

      <!-----------* EMPTY STATE *--------------->
      <div class="no_cards" v-if="errorTags.length == 0">
        <p class="no_cards_text">
          Error cards will be generated automatically by the system or can be
          marked by an operator.
        </p>
      </div>

      <!-----------* NON-EMPTY STATES *--------------->
      <div class="has_cards" v-else>
        <!-----------* display by categories *--------------->
        <div class="category_accordion_container" v-if="State == 'Category'">
          <p class="sort_type_text">Sorted by: Category</p>
          <div v-for="category in this.categories" v-bind:key="category">
            <hr class="accordion_lines" />
            <AccordionWithHeader
              class="accordion_with_header"
              :header="category"
              :contents="
                checkFilter(errorTags).filter(
                  (errorTag) => errorTag.category == category
                )
              "
            />
          </div>
          <hr class="accordion_lines" />
        </div>

        <!-----------* display by default *--------------->
        <div class="cards" v-if="State == 'Default'">
          <div
            class="errorTagsWrapper"
            v-for="errorTag in checkFilter(errorTags)"
            v-bind:key="errorTag"
          >
            <ErrorTag :errorTag="errorTag" class="indivErrorTag" />
          </div>
        </div>
      </div>
    </div>
  </div>
  <!-- overall container -->
</template>

<script>
import ErrorLogDropdown from "./ErrorLogDropdown";
import ErrorLogSearch from "./ErrorLogSearch";
import ErrorTag from "./ErrorTagCosmos";
import PopUpDropdown from "./PopUpDropdown";
import AccordionWithHeader from "./ErrorTagAccordion";
import $ from "jquery";
import CosmosIconPack from "@/styles/CosmosIconPack.js";

export default {
  name: "ErrorLogFilter",

  components: {
    ErrorLogDropdown,
    ErrorLogSearch,
    ErrorTag,
    PopUpDropdown,
    AccordionWithHeader,
  },

  props: {
    errorTags: {
      type: Array,
      required: true,
    },
  },

  methods: {
    sortCard(sortBy) {
      if (sortBy == "None") {
        this.State = "Default";
      } else if (sortBy == "Priority(Low - High)") {
        this.State = "Priority";
        this.sortBy = "LowToHigh";
      } else if (sortBy == "Priority(High - Low)") {
        this.State = "Priority";
        this.sortBy = "HighToLow";
      } else if (sortBy == "Last Modified") {
        this.errorTags.sort((a, b) =>
          a.lastModified < b.lastModified ? 1 : -1
        );
        this.State = "Default";
      } else if (sortBy == "Category") {
        this.State = "Category";
      } else if (sortBy == "Created(New - Old)") {
        this.errorTags.sort((a, b) => (a.created < b.created ? 1 : -1));
        this.State = "Default";
      } else if (sortBy == "Created(Old - New)") {
        this.errorTags.sort((a, b) => (a.created >= b.created ? 1 : -1));
        this.State = "Default";
      }
    },
    searchCategoryUpdate(category) {
      console.log("received search category update: " + category);
      if (category == "Error Tag") {
        this.searchCategory = "tags";
      } else if (category == "Error ID") {
        this.searchCategory = "id";
      } else if (category == "Sensor Name") {
        this.searchCategory = "sensors";
      } else if (category == "Mission Time Frame (M+)") {
        this.searchCategory = "timeframe";
      } else if (category == "Date Created") {
        this.searchCategory = "created";
      } else if (category == "Date Last Modified") {
        this.searchCategory = "lastModified";
      } else if (category == "Category") {
        this.searchCategory = "category";
      }
      this.updateValidTerms();
    },
    updateValidTerms() {
      let s = this.searchCategory;
      // string values
      if (
        s == "category" ||
        s == "id" ||
        s == "created" ||
        s == "lastModified"
      ) {
        let arr = [];
        this.errorTags.forEach((tag) => arr.push(tag[s]));
        this.validTerms = [...new Set(arr)];
      }
      // arrays
      else if (s == "tags" || s == "sensors") {
        let arr = [];
        this.errorTags.forEach((tag) => (arr = arr.concat(tag[s])));
        this.validTerms = [...new Set(arr)];
      }
      // time interval
      // *****************    TO-DO: Use clock   ******************
      else if (s == "timeframe") {
        console.log("sort by time frame");
      }
    },
    filterCardBySearch(searchedTerms) {
      console.log("updated searchedTerms: " + searchedTerms);
      let sc = this.searchCategory;
      console.log(sc);
      if (
        sc == "category" ||
        sc == "id" ||
        sc == "created" ||
        sc == "lastModified"
      ) {
        let tmp = this.errorTags;
        tmp = tmp.filter((tag) => searchedTerms.includes(tag[sc]));
        this.filteredTags = tmp;
      }
    },
    checkFilter(rawErrorTags) {
      let tags =
        this.filteredTags.length != 0 ? this.filteredTags : rawErrorTags;
      return tags;
    },
    expandAllAccordion() {
      let accordions = $(".accordion_with_header");
      // console.log(accordions);
      [...accordions].forEach((el) => el.trigger("click"));
      //  [...$(".accordion_with_header ")].forEach((el) => el.trigger("click"));

      console.log("expand all accordion");
    },
  },
  data() {
    return {
      validTerms: [],
      sortPopUpOpen: false,
      CosmosIconPack,
      sortOptions: [
        "Last Modified",
        "Created(New - Old)",
        "Created(Old - New)",
        "Category",
        "Priority(Low - High)",
        "Priority(High - Low)",
        "None",
      ],
      sortTitle: "Sort By",
      sortBy: "LowToHigh",
      categories: [
        "Acceleration",
        "Communication",
        "Motor",
        "Power",
        "Temperature",
      ],
      searchCategory: "Error Tag",
      priorities: ["Low", "Medium", "High"],
      fixed_header_priority: " Priority",
      filteredTags: [],
      State: "Priority",
    };
  },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";

* {
  font-family: "Barlow";
}

.error-log-filter {
  width: 350px;
  height: 800px;
  overflow-y: scroll;
  background-color: $color-near-black;
}

.error-log-filter::-webkit-scrollbar {
  background-color: $color-near-black;
  width: 0.5em;
}
.error-log-filter::-webkit-scrollbar-track {
  -webkit-box-shadow: inset 0 0 6px rgba(0, 0, 0, 0.3);
}

.error-log-filter::-webkit-scrollbar-thumb {
  background-color: $color-grey3;
  border-radius: 30px;
}

.top_bar {
  //z-index: -1111111;
  //position: absolute;
}

.row {
  display: flex;
  text-align: center;
}

.relative__parent {
  position: relative;
}

.fixed__child {
  position: fixed;
  left: 50px;
  top: 20px;
}

.expand_text {
  font-size: 0.9em;
  float: right;
  text-align: right;
  margin-left: 20%;
  display: block;
  margin-top: 4%;
}

.centered_content {
  overflow: hidden;
  width: auto !important;
  overflow-x: hidden !important;
  margin-left: 8%;
  background-color: $color-near-black;
}

.header {
  display: flex;
  flex-direction: row;
  align-items: center;
}

.header_title {
  font-weight: 1000;
  font-size: 1em;
}

.errorTagsWrapper {
  overflow: scroll;
  background-color: $color-near-black;
}

.indivErrorTag {
  background-color: $color-grey5;
  padding-top: 0.5em;
  padding-bottom: 0.5em;
}

.popup .sort_popup_content {
  width: 40%;
  float: right;
  margin-right: 10%;
  z-index: 111111;
  //display: block;
}

.sort_popup_content {
  font-size: 0.8em;
  z-index: 1111;
}

.no_cards {
  text-align: center;
  padding-top: 30%;
  margin: auto;
  width: 45%;
}

.no_cards_text {
  color: $color-grey2;
  font-size: 0.8em;
}

.sort_type_text {
  font-size: 0.8em;
  color: $color-grey2;
}

.accordion_lines {
  border: none;
  height: 1px;
  width: 90%;
  margin-left: 0;
  margin-top: 0;
  margin-bottom: 0;
  padding: 0;
  /* Set the hr color */
  color: $color-grey4; /* old IE */
  background-color: $color-grey4; /* Modern Browsers */
}

.unselectable {
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  -khtml-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
}

.header_icons {
  margin-left: 50%;
  display: flex;
}

.icon {
  width: 16px;
  height: 16px;
  stroke: white;
  padding-left: 0.5em;
}

.sort_icon {
  stroke-width: 1.4px;
}

.new_window_icon {
  stroke-width: 0.7px;
}
</style>
