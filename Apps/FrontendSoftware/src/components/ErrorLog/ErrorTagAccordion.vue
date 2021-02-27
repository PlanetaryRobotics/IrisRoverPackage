<template>
  <div class="accordion">
    <div
      class="accordion-header"
      :class="this.opened ? 'active' : ''"
      @click="toggleAccordion"
      v-on:expandall:fromparent="toggleAccordion"
    >
      <!----- TODO: not aligned ---->
      <p class="header-text inline">
        {{ this.header }} {{ this.fixed_header }}
      </p>
      <p class="arrow inline align-right"></p>
    </div>

    <div class="accordion-body" :class="this.opened ? 'open' : 'collapse'">
      <div
        class="errorTagsWrapper"
        v-for="errorTag in this.contents"
        v-bind:key="errorTag.priority"
      >
        <ErrorTag :errorTag="errorTag" class="indivErrorTag" />
      </div>
    </div>
  </div>
  <!-- root container -->
</template>

<script>
import ErrorTag from "./ErrorTagCosmos";

export default {
  name: "AccordionWithHeader",
  components: {
    ErrorTag,
  },
  props: {
    header: {
      type: String,
      required: true,
      default: "",
    },
    contents: {
      type: Array,
      required: true,
    },
    fixed_header: {
      type: String,
      required: false,
      default: "",
    },
  },
  data() {
    return {
      opened: false,
    };
  },
  methods: {
    toggleAccordion() {
      console.log("here");
      this.opened = !this.opened;
    },
  },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";
@import "@/styles/_functional.scss";

* {
  font-family: "Barlow";
  color: white;
}

.header-text {
  font-weight: 1000;
}

.align-right {
  display: inline-block;
  text-align: right;
}

.accordion-header {
  padding-bottom: 1em;
  padding-top: 1em;
  cursor: pointer;

  color: #ffffff;
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
    bottom: 0;
    padding: 0;
    margin: 0;
    -webkit-transition: 0.4s ease;
    transition: 0.4s ease;
    text-align: left;
    -webkit-transform: rotate(45deg);
    transform: rotate(45deg);
    //float: right;
    display: inline-block;
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

.inline {
  display: inline;
}

.open {
  display: block;
}
.collapse {
  display: none;
}
</style>
