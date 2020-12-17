<template>
  <div class="error-log-filter">
    <div class="centered_content">
      <!--------------------- TOP SECTION ------------------------->
      <div class="header">
        <p class="header_title">Errors Log</p>

        <!-- **********************************
            TODO: Header Icons
            - Change to stored UI Icons
           **********************************
      -->
        <div class="header_icons">
          <span
            @click="
              let original = sortPopUpOpen;
              sortPopUpOpen = !original;
            "
          >
            &#8693;
          </span>
          <span> &#x21f1; </span>
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

      <div class="top_bar">
        <ErrorLogDropdown />

        <!-- **********************************
            TODO:  ErroLogSearch
            - Key down not working (click works)
            - Dropdown hover color background not full 
            - Functionality: Enter to filter out error cards,
                             Need further clarifications on that
           **********************************
      -->
        <ErrorLogSearch :validTerms="validTerms" />
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
                errorTags.filter((errorTag) => errorTag.priority == priority)
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
                errorTags.filter((errorTag) => errorTag.priority == priority)
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
                errorTags.filter((errorTag) => errorTag.category == category)
              "
            />
          </div>
          <hr class="accordion_lines" />
        </div>

        <!-----------* display by default *--------------->
        <div class="cards" v-if="State == 'Default'">
          <div
            class="errorTagsWrapper"
            v-for="errorTag in this.errorTags"
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

export default {
  name: "ErrorLogFilter",

  components: {
    ErrorLogDropdown,
    ErrorLogSearch,
    ErrorTag,
    PopUpDropdown,
    AccordionWithHeader,
  },

  props: {},

  methods: {
    sortCard(sortBy) {
      if (sortBy == "Priority(Low - High)") {
        this.State = "Priority";
        this.sortBy = "LowToHigh";
      } else if (sortBy == "Priority(High - Low)") {
        this.State = "Priority";
        this.sortBy = "HighToLow";
      } else if (sortBy == "Last Modified") {
        this.errorTags.sort((a, b) => (a.modified < b.modified ? 1 : -1));
        this.State = "Default";
      } else if (sortBy == "Category") {
        this.State = "Category";
      } else if (sortBy == "Created(New - Old)") {
        this.errorTags.sort((a, b) => (a.created < b.created ? 1 : -1));
        this.State = "Default";
      } else if (sortBy == "Created(Old - New)") {
        this.errorTags.sort((a, b) => (a.create >= b.created ? 1 : -1));
        this.State = "Default";
      }
    },
    expandAllAccordion() {
      let accordions = $(".accordion_with_header");
      console.log(accordions);
      [...accordions].forEach((el) => el.click());
      console.log("expand all accordion");
    },
  },

  data() {
    return {
      validTerms: ["Sensor1", "Sensor2"],
      sortPopUpOpen: false,
      sortOptions: [
        "Last Modified",
        "Created(New - Old)",
        "Created(Old - New)",
        "Category",
        "Priority(Low - High)",
        "Priority(High - Low)",
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
      priorities: ["Low", "Medium", "High"],
      fixed_header_priority: " Priority",
      errorTags: [
        {
          max: 5,
          id: "A014",
          priority: "Low",
          name: [
            "MotorSensor1",
            "MotorSensor2",
            " MotorSensor3",
            " MotorSensor4",
            "MotorSensor5",
          ],
          summary: "Motor Overheating(2)",
          notes:
            "temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.",
          tags: "#TempError",
          timeFrame: ["AO 10-10-2020 20:05:00", ""],
          category: "Motor",
        },
        {
          id: "M001",
          priority: "Low",
          name: [
            "MotorSensor1",
            "MotorSensor2",
            " MotorSensor3",
            " MotorSensor4",
            "MotorSensor5",
          ],
          summary: "Motor Overheating(2)",
          notes:
            "temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.",
          tags: "#TempError",
          timeFrame: ["AO 10-10-2020 20:05:00", ""],
          category: "Motor",
        },
        {
          id: "M002",
          priority: "Low",
          name: [
            "MotorSensor1",
            "MotorSensor2",
            " MotorSensor3",
            " MotorSensor4",
            "MotorSensor5",
          ],
          summary: "Motor Overheating(2)",
          notes:
            "temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.",
          tags: "#TempError",
          timeFrame: ["AO 10-10-2020 20:05:00", ""],
          category: "Acceleration",
        },
        {
          id: "A015",
          priority: "Medium",
          name: [
            "MotorSensor1",
            "MotorSensor2",
            " MotorSensor3",
            " MotorSensor4",
            "MotorSensor5",
          ],
          summary: "Motor Overheating(2)",
          notes:
            "temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.",
          tags: "#TempError",
          timeFrame: ["AO 10-10-2020 20:05:00", ""],
          category: "Power",
        },
        {
          id: "M003",
          priority: "Medium",
          name: [
            "MotorSensor1",
            "MotorSensor2",
            " MotorSensor3",
            " MotorSensor4",
            "MotorSensor5",
          ],
          summary: "Motor Overheating(2)",
          notes:
            "temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.",
          tags: "#TempError",
          timeFrame: ["AO 10-10-2020 20:05:00", ""],
          category: "Communication",
        },
        {
          id: "M009",
          priority: "High",
          name: [
            "MotorSensor1",
            "MotorSensor2",
            " MotorSensor3",
            " MotorSensor4",
            "MotorSensor5",
          ],
          summary: "Motor Overheating(2)",
          notes:
            "temp sensor 2 shows overheating near motor 2. Second time this has happened in 1 hour.",
          tags: "#TempError",
          timeFrame: ["AO 10-10-2020 20:05:00", ""],
          category: "Temperature",
        },
      ],
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
  background: $color-near-black;
}

.top_bar {
  //position: absolute;
  z-index: -1;
}

.row {
  display: flex;
  text-align: center;
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
}

.header {
  display: flex;
  flex-direction: row;
  align-items: center;
}

.header_icons {
  margin-left: 65%;
}

.header_title {
  font-weight: 1000;
  font-size: 1em;
}

.errorTagsWrapper {
  overflow: scroll;
}

.indivErrorTag {
  background-color: $color-grey5;
  padding-top: 0.5em;
  padding-bottom: 0.5em;
  width: 90%;
}

.popup {
  width: 40%;
  z-index: 1;
  float: right;
  margin-right: 10%;
  display: block;
  //position: fixed; /* Stay in place */
}

.sort_popup_content {
  font-size: 0.8em;
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
  width: 90%;
  text-align: left;
  margin-left: 0;
  color: $color-near-black;
  opacity: 0.5;
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
