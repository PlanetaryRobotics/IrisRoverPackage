<!--
A collection of tabs for making adjustments to images and adding or tracking features. 

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 3/31/2019, Moosbrugger
-->

<template>
  <div class="functional">
    <!-- CONTAINER FOR EACH TAB'S CONTENT -->
    <div class="functional__open">
      <FunctionalEdit v-show="isEdit"/>
      <FunctionalFeatures v-show="isFeatures"/>
    </div>
    <!-- SIDE MENU -->
    <div class="menu">
        <div class="menu__option" v-bind:class="{ selected: isEdit }" @click="selectEdit">
          <svg width="17" height="16" viewBox="0 0 17 16" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon" v-bind:class="{ icon__selected: isEdit }"><line x1="4.75" y1="2.6875" x2="16.5" y2="2.6875" stroke-linecap="round"/><line x1="0.5" y1="8" x2="12.25" y2="8" stroke-linecap="round"/><line x1="0.5" y1="13.3125" x2="6.9375" y2="13.3125" stroke-linecap="round"/><line x1="11.125" y1="13.3125" x2="16.5" y2="13.3125" stroke-linecap="round"/><circle cx="2.65625" cy="2.65625" r="2.15625"/><circle cx="14.3438" cy="7.96875" r="2.15625"/><circle cx="9.03125" cy="13.2812" r="2.15625"/></svg>
        </div>
        <div class="menu__option" v-bind:class="{ selected: isFeatures }" @click="selectFeatures">
          <svg width="13" height="19" viewBox="0 0 13 19" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon" v-bind:class="{ icon__selected: isFeatures }"><circle cx="6.5" cy="6.5" r="2"/><path d="M1 7C1 11 4.5 18 6.5 18C8.5 18 12 11 12 7C12 4 10.5 1 6.5 1C2.5 1 1 4 1 7Z"/></svg>
        </div>
    </div>
  </div>
</template>

<script>
import FunctionalEdit from "@/components/ImageViewer/functional/FunctionalEdit.vue"
import FunctionalFeatures from "@/components/ImageViewer/functional/FunctionalFeatures.vue"

export default {
  components: {
    FunctionalEdit,
    FunctionalFeatures
  },
  data() {
    return {
      isEdit: true, //      - Bool controlls if Edit menu is open
      isFeatures: false //  - Bool controlls if Features menu is open
    }
  },
  methods: {
    selectFeatures: function() { // Opens the features menu, or if it is already open closes it.
      if (this.isFeatures === true) {
        this.isFeatures = false
      }
      else {
        this.isEdit = false
        this.isFeatures = true
      }
    },
    selectEdit: function() {
      if (this.isEdit === true) { // Opens the edit menu, or if it is already open closes it.
        this.isEdit = false
      }
      else {
        this.isFeatures = false
        this.isEdit = true
      }
    }
  }
}
</script>


<style lang="scss" scoped>
@import '@/styles/_functional.scss';

.functional {
  display: grid;
  user-select: none;
  grid-template-columns: min-content min-content;

  .menu {
    padding-top: 1rem;
    border-left: 1.4px solid $color-near-black;

    &__option {
      padding: 1.2rem 1rem;
      cursor: pointer;
    }
  }
}

.icon {
  stroke: $color-grey-dark;
  width: 3rem;
  transition: .1s ease-in-out;

  &__selected {
    stroke: $color-grey-light;
  }
}
</style>

