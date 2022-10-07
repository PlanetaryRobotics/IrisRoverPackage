<!--
Provides tools for editing the image including adjustments, filters and a histogram.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 10/6/2019, Colombo
-->

<template>
  <div
    id="functionalEdit"
    class="scrollable"
  >
    <!-- HISTOGRAM -->
    <div class="editTab">
      <div
        class="editTab__header"
        @click="toggleHistogram"
      >
        <svg
          width="14"
          height="7"
          viewBox="0 0 8 4"
          fill="none"
          xmlns="http://www.w3.org/2000/svg"
          class="editTab__icon"
          :class="{ open : show.histogram }"
        >
          <path
            d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5"
            stroke-linecap="round"
          />
        </svg>
        <h2 class="text__main--bold editTab__title">
          Histogram
        </h2>
      </div>
      <div
        v-show="show.histogram"
        class="editTab__content"
      >
        <img
          class="histogram"
          src="~@/assets/gifs/egg/dumpster.gif"
        >
        [Histogram goes here ;]
      </div>
    </div>
    <!-- ADJUSTMENTS -->
    <div class="editTab">
      <div
        class="editTab__header"
        @click="toggleAdjustments"
      >
        <svg
          width="14"
          height="7"
          viewBox="0 0 8 4"
          fill="none"
          xmlns="http://www.w3.org/2000/svg"
          class="editTab__icon"
          :class="{ open : show.adjustments }"
        >
          <path
            d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5"
            stroke-linecap="round"
          />
        </svg>
        <h2 class="text__main--bold editTab__title">
          Adjustments
        </h2>
      </div>
      <div
        v-show="show.adjustments"
        class="editTab__content"
      >
        <functional-adjustments />
      </div>
    </div>
    <!-- PRESETS -->
    <div class="editTab">
      <div
        class="editTab__header"
        @click="togglePresets"
      >
        <svg
          width="14"
          height="7"
          viewBox="0 0 8 4"
          fill="none"
          xmlns="http://www.w3.org/2000/svg"
          class="editTab__icon"
          :class="{ open : show.presets }"
        >
          <path
            d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5"
            stroke-linecap="round"
          />
        </svg>
        <h2 class="text__main--bold editTab__title">
          Preset Adjustments
        </h2>
      </div>
      <Presets
        v-show="show.presets"
        class="editTab__content"
      />
    </div>
  </div>
</template>

<script>
import FunctionalAdjustments from '@/components/ImageViewer/functional/FunctionalAdjustments.vue';
import Presets from '@/components/ImageViewer/functional/Presets.vue';

export default {
    components: {
        FunctionalAdjustments,
        Presets
    },
    data() {
        return {
            show: {
                histogram: false, //    - Controls visibility of the histogram tab
                adjustments: true, //   - Controls visibility of adjustments tab
                presets: true //        - Controls visibility of presets tab
            }
        };
    },
    methods: {
        toggleHistogram() { // Toggles visibility of the histogram tab
            this.show.histogram = !this.show.histogram;
        },
        toggleAdjustments() { // Toggles the visibility of the adjustments tab
            this.show.adjustments = !this.show.adjustments;
        },
        togglePresets() { // Toggles the visibility of the presets tab
            this.show.presets = !this.show.presets;
        }
    }
};
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

#functionalEdit {
  width: 25rem;
  max-width: 100%;
  max-height: 100%;
  height: 100%;
  background-color: $color-background;
}

.editTab {
  border-bottom: 1px solid $color-near-black;
  padding: 1.5rem 2rem;

  &__header {
    display: flex;
    align-items: center;
    cursor: pointer;
    user-select: none;
  }

  &__icon {
    stroke: $color-grey-light;
    width: 14px;
    height: 14px;
    transform: rotate(-90deg);
    transition: .1s ease-in-out;
  }

  &__title {
    margin-left: 1rem;
  }

  .histogram {
    max-width: 100%;
    width: 100%;
    height: auto;
    object-fit: contain;
    padding-top: 1.5rem;
  }
}

.open {
  transform: rotate(0deg);
}
</style>
