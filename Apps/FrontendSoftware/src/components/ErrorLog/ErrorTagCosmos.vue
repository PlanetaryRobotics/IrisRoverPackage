<template id="errorTag">
  <div class="errorTag">
    <div class="row errorTag--title" @click="toggleDetail">
      <svg width="12" height="16" class="">
        <rect
          v-for="i in this.priorityValue"
          v-bind:key="i"
          width="2"
          height="14"
          style="fill: #f00"
          :x="(i - 1) * 4"
        />
      </svg>

      <div class="tagName">{{ errorTag.id }}</div>

      <div class="col category row" :class="this.opened ? 'active' : ''">
        <span class="category_text"> Category: {{ errorTag.category }} </span>

        <span class="arrow"></span>
      </div>
    </div>

    <div class="row">
      <p class="inline title_text">
        Sensor Name ({{ errorTag.sensors.length }}): &nbsp;
      </p>
      <p v-for="sensor in errorTag.sensors" v-bind:key="sensor" class="inline">
        {{ sensor }}
      </p>
    </div>

    <div class="row">
      <p class="inline title_text">Time Frame (M+): &nbsp;</p>
      <p class="inline">{{ errorTag.start }} - {{ errorTag.end }}</p>
    </div>

    <div class="details" :class="this.opened ? 'open' : 'collapse'">
      <div class="row">
        <p class="inline title_text">Summary:&nbsp;</p>
        <p class="inline">{{ errorTag.summary }}</p>
      </div>

      <div class="row">
        <p class="inline title_text">Notes:&nbsp;</p>
        <p class="inline">{{ errorTag.notes }}</p>
      </div>

      <div class="row">
        <p class="inline title_text">Tags:&nbsp;</p>

        <p v-for="tag in errorTag.tags" v-bind:key="tag" class="inline">
          {{ tag }}
        </p>
      </div>

      <div class="row">
        <p class="inline title_text">Created:&nbsp;</p>
        <p class="inline">{{ errorTag.created }}</p>
      </div>

      <div class="row">
        <p class="inline title_text">Last modified:&nbsp;</p>
        <p class="inline">{{ errorTag.lastModified }}</p>
      </div>
    </div>

    <div class="row viewButton" @click="viewAnalysis(errorTag)">
      <span class="viewButtonText">VIEW ANALYSIS</span>
    </div>
  </div>
</template>

<script>
// change name to ErrorTagCosmos or sth

export default {
  name: "ErrorTag",

  props: ["errorTag", "openAnalysis", "index"],

  mounted() {
    this.setPriority();
  },

  methods: {
    viewAnalysis(errorTag) {
      console.log(errorTag); // so vue doesn't scream

      this.$emit("pushTag");
    },

    setPriority() {
      if (this.errorTag.priority === "Low") {
        this.priorityValue = 1;
      } else if (this.errorTag.priority === "Mid") {
        this.priorityValue = 2;
      } else {
        this.priorityValue = 3;
      }
    },

    toggleDetail() {
      this.opened = !this.opened;
    },
  },

  data() {
    return {
      priorityValue: 0,

      opened: false,
    };
  },
};
</script>

<style scoped lang="scss">
@import "@/styles/_functional.scss";

.errorTag {
  position: relative;
  border-radius: 4px;
  padding-left: 16px;
  padding-top: 4px;
  padding-bottom: 4px;
  padding-right: 4px;
  font-size: 1.1rem;
  line-height: 12px;
  margin-bottom: 8px;
  background-color: $color-grey5;
  width: 85%;

  .row {
    margin-bottom: 8px;
    color: white;
    font-size: 0.7em;
    line-height: 1.2em;
  }

  span {
    font-weight: 600;
  }

  .errorTag--title {
    margin-left: -16px;
  }

  .category {
    text-align: right;
    padding-right: 25px;
    font-weight: 500;
    font-size: 0.8rem;
    line-height: 10px;
    color: #ffffff;
    margin-bottom: 8px;
    transition: 0.15s ease;
    -webkit-transition: 0.15s ease;

    &.active .arrow {
      -webkit-transform: rotate(45deg) translate(-4px, -4px);
      transform: rotate(45deg) translate(-4px, -4px);
    }

    &.active .arrow:before {
      -webkit-transform: translate(8px, 0);
      transform: translate(8px, 0);
    }

    &.active .arrow:after {
      -webkit-transform: rotate(90deg) translate(8px, 0);
      transform: rotate(90deg) translate(8px, 0);
    }

    .arrow {
      width: 8px;
      height: 8px;
      display: inline-block;
      position: relative;
      bottom: 0;
      left: 5px;
      -webkit-transition: 0.4s ease;
      transition: 0.4s ease;
      margin-top: 2px;
      text-align: left;
      -webkit-transform: rotate(45deg);
      transform: rotate(45deg);
      float: right;
    }

    .arrow:before,
    .arrow:after {
      position: absolute;
      content: "";
      display: inline-block;
      width: 8px;
      height: 1px;
      background-color: #fff;
      -webkit-transition: 0.4s ease;
      transition: 0.4s ease;
    }

    .arrow:after {
      position: absolute;
      -webkit-transform: rotate(90deg);
      transform: rotate(90deg);
      top: -4px;
      left: 4px;
    }
  }

  .tagName {
    color: #ff2f46;
  }

  .title_text {
    font-weight: 400;
  }
  .category_text {
    font-size: 0.8em;
    font-weight: 100;
  }

  .viewButton {
    cursor: pointer;
    padding-top: 12px;
    /*padding-bottom: 12px;*/
    clear: both;
  }

  .viewButtonText {
    /* Primary */
    color: #8a6dff;
  }

  .open {
    display: block;
  }

  .collapse {
    display: none;
  }

  .inline {
    display: inline;
  }
}
</style>
