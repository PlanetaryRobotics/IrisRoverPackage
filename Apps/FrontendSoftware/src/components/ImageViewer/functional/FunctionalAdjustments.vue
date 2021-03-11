<!--
Slider adjustments of the image. Can be saved as presets.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 08/03/2020, Colombo
-->

<template>
  <div id="functionalAdjustments">
    <!-- SLIDER ELEMENTS -->
    <div
      v-for="(key, index) in adjustmentKeys"
      :key="index"
      class="functionalSlider"
    >
      <!-- HEADER -->
      <div class="functionalSlider__header">
        <p class="functionalSlider__header--title text__small">
          {{ key }}
        </p>
        <p class="functionalSlider__header--value text__small">
          {{ editData.adjustments[key] }}
        </p>
      </div>
      <!-- INPUT -->
      <input
        v-model="editData.adjustments[key]"
        type="range"
        min="-1"
        max="1"
        class="slider"
        step=".02"
      >
    </div>

    <!-- FILTER SAVE -->
    <div class="functionalAdjustments__input">
      <input 
        v-model="editData.name"
        type="text"
        class="input__text--small text__main"
        placeholder="Preset Name"
        @keyup.stop
        @keydown.stop
      >
      <button
        class="button button__subtle text__smallCaps--bold"
        @click="createPreset"
      >
        SAVE
      </button>
    </div>
    <p class="text__small">
      {{ inputError }}
    </p>
  </div>
</template>

<script>

import { mapState } from 'vuex';

export default {
    data() {
        return {
            inputError: '',
            adjustmentKeys: []
        };
    },
    computed: {
        ...mapState({
            editData: state => state.IMG.adjustmentsEditorState
        })
    },
    mounted: function() {
        this.resetAdjustments();
        this.adjustmentKeys = Object.keys(this.editData.adjustments);
    },
    methods: {
        createPreset: function() { // Formats data for the filter class input
            //   this.$store.commit('CREATE_PRESET', newPreset) // tells Vuex to create new Filter class
            //   this.resetAdjustments() // Resets adjustments to default parameters

            // Checks to see if values are in bounds and non-zero
            let isInput = 0;
            for ( const key in this.editData.adjustments ) {
                this.editData.adjustments[key] = Number(this.editData.adjustments[key]); // changes input from string to number

                const adjustmentVal = this.editData.adjustments[key];

                if (!( -1 < adjustmentVal && adjustmentVal < 1 )) { // checks that data is within boundry conditions
                    console.error('The following adjustment value was out of bounds');
                    console.log(key, ': ', adjustmentVal);
                    return;
                }
                else if ( adjustmentVal === 0 ) { // increments if value is set to default and checks to make sure changes have been applied
                    isInput++;
                }
            }
            if ( isInput >= this.adjustmentKeys.length ) {
                this.inputError = 'All adjustments are set to \'0\'';
                console.error('All adjustments are set to \'0\'. Did not create Preset.');
                return;
            }

            // Validates name input is not empty and is shorter than 30 characters
            if (this.editData.name === '') {
                this.inputError = 'Preset name cannot be empty';
                console.error('Preset name cannot be empty. Did not create Preset.');
                return;
            }
            else if (this.editData.name.length > 30) {
                this.inputError = 'Preset name too long.';
                console.error('Preset name too long, cannot be longer than 30 characters. Did not create Preset.');
                return;
            }

            // Creates new Preset
            this.$store.commit('CREATE_PRESET', this.editData);
            // resets adjustments
            this.resetAdjustments();
        },
        resetAdjustments: function() { // resets default data after creating preset or initial load
            this.$store.commit('RESET_ADJUSTMENTS');
            this.inputError = '';
        }
    }
};
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';
.functionalSlider {
  margin-top: 2rem;

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
  transition: all .6s ease;

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

.functionalAdjustments {
  &__input {
    margin-top: 3rem;
    margin-bottom: 1rem;
    display: flex;
    align-items: center;
    justify-content: space-between;
  }
}
</style>
