<!--
A single instance of a slider for the adjustments tab.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 3/31/2019, Moosbrugger
-->

<template>
  <div id="functionalSlider">
    <!-- HEADER -->
    <div class="functionalSlider__header">
      <p class="functionalSlider__header--title text__small">
        {{ sData.name }}
      </p>
      <p class="functionalSlider__header--value text__small">
        {{ output }}
      </p>
    </div>
    <!-- INPUT -->
    <input
      v-model="value"
      type="range"
      min="0"
      max="200"
      class="slider"
    >
  </div>
</template>

<script>
export default {
    props: {
        sData: {
            type: Object
        }
    },
    data() {
        return {
            value: 100 //     - Default value for slider (range of 0 - 200)
        };
    },
    computed: {
        output() { // sets the output of the slider, some near 0 values are crushed for usability.
            let output = (this.value - 100) / 100;
            if (output >= -.01 && output <= .01) {
                output = 0;
            }
            return output;
        }
    }
};
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

#functionalSlider {
  margin-top: 1.5rem;
}

.functionalSlider {
  &__header {
    display: flex;
    justify-content: space-between;
  }
}

.slider {
  appearance: none;
  height: 4px;
  border-radius: 2px;
  background: $color-grey-dark;
  outline: none;
  width: 100%;

  &::-webkit-slider-thumb {
    -webkit-appearance: none;
    appearance: none;
    width: 1.3rem;
    height: 1.3rem;
    background: $color-primary;
    cursor: pointer;
    border-radius: 50%;
  }
}

</style>
