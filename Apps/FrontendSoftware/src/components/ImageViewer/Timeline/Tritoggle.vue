<!--
Basic Three-State Toggle to Select from which Camera Images are Displayed.

Author: Connor W. Colombo, CMU
Created: 10/6/2019
Last Updated: 08/14/2020, Colombo
-->
<template>
  <div id="tritoggle">
    <input
      style="grid-area: F"
      type="radio"
      name="tritoggle"
      value="F"
      class="tooltip"
      title="Show Images from Front Camera"
      @click="buttonClicked($event, 'F')"
      @keydown.prevent="/*prevent radio button from changing selection on arrow keydown*/"
    >
    <label style="grid-area: F">F</label>
    <input
      checked
      style="grid-area: B"
      type="radio"
      name="tritoggle"
      value="B"
      class="tooltip"
      title="Show Images from Both Cameras"
      @click="buttonClicked($event, 'B')"
      @keydown.prevent="/*prevent radio button from changing selection on arrow keydown*/"
    >
    <svg
      style="grid-area: B"
      :class="{highlighted: selection=='B'}"
      class="selectable-vector icon__dualCamera"
      viewBox="0 0 16 19"
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
    >
      <path
        d="M15 1H1L5.03846 8.19231H8H10.9615L13.3846 3.94231L15 1Z"
        stroke-linecap="round"
        stroke-linejoin="round"
      />
      <path
        d="M15 18H1L5.03846 10.8077H10.9615L15 18Z"
        stroke-linecap="round"
        stroke-linejoin="round"
      />
    </svg>
    <input
      style="grid-area: R"
      type="radio"
      name="tritoggle"
      value="R"
      class="tooltip"
      title="Show Images from Rear Camera"
      @click="buttonClicked($event, 'R')"
      @keydown.prevent="/*prevent radio button from changing selection on arrow keydown*/"
    >
    <label style="grid-area: R">R</label>
    <!-- Indicator Nub: -->
    <span />
  </div>
</template>

<script>
import TooltipEquip from '@/styles/TooltipEquip.js';

export default {
    name: 'Tritoggle',
    data: function(){
        return {
            selection: 'B' // default selection is both cameras
        };
    },
    mounted(){
        TooltipEquip(this.$el);
    },
    methods: {
    // Select Camera based on Which Button was Clicked:
        buttonClicked: function(event, camera){
            let cameraNames = {
                F: 'Front',
                B: 'Both',
                R: 'Rear'
            };
            this.selection = camera;
            this.$store.state.IMG.cameraSelection = cameraNames[camera];
        }
    }
};
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';
@import '@/styles/_colors.scss';

$spacing: 2.4rem;

#tritoggle {
  user-select: none;

  /* Component Base Styling: */
  background: $color-grey-dark;
  width: 3 * $spacing;
  height: 1 * $spacing;
  border-radius: $spacing / 2;
  position:relative;
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  grid-template-rows: min-content;
  grid-template-areas: "F B R";
  align-items: center;
  justify-content: space-between;
  justify-items: center;

  /* Bead: */
  span {
    position:absolute;
    top: 0;
    display:block;
    background: $color-primary;
    color: $color-near-white;
    height: $spacing;
    width: $spacing;
    line-height: $spacing;
    border-radius: $spacing / 2;
    transition:all 0.3s ease-in-out;
  }

  /* Selectors: */
  input {
    z-index: 3;
    opacity: 0;
    cursor: pointer;
  }

  label,svg {
    height: $spacing;
    width: $spacing / 2;
    line-height: $spacing;
    text-align: center;
    vertical-align: center;
    color: $color-grey;
    stroke: $color-grey;
    position: relative;
    z-index: 2;
    font-size: 1.2rem;
    font-weight: bold;
  }

  .icon__dualCamera {
    fill: none;
    stroke-width: 1.5px;
    height: 1.2rem;
    width: auto;
  }

  /* Selection Movement: */
  input[value="F"]:checked~span {
    left: 0rem;
  }
  input[value="B"]:checked~span {
    left: $spacing;
  }
  input[value="R"]:checked~span {
    left: 2 * $spacing;
  }
  input:checked+label {
    color: $color-near-white;
  }
  input:checked+svg {
    stroke: $color-near-white;
  }

}
</style>
