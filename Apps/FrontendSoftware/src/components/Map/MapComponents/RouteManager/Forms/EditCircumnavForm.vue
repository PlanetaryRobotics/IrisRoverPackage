<template>
  <div>
    <!-- SEGMENT NAME -->
    <div class="segName">
      <div class="segName__label formLabel">POI*</div>
      <div class="segName__name">{{POIName}}</div>
    </div>
    
    <!-- DIRECTION -->
    <div class="direction">
      <div class="direction__label formLabel">Direction*</div>
      <div class="direction__container">
        <div class="direction__button--left" :class="{selected: formValues.ISCLOCKWISE}" @click.capture="toggleDirection('clockwise')">
          Clockwise
        </div>
        <div class="direction__button--right" :class="{selected: !formValues.ISCLOCKWISE}" @click.capture="toggleDirection('counter')">
          Counter
        </div>
      </div>
    </div>

    <!-- RADIUS -->
    <div class="radius">
      <label class="formLabel">Radius* </label>
      <input class="radius__input" 
              type="text"
              placeholder="Enter radius in cm"
              v-model="formValues.RADIUS"
              @input="validateInput('RADIUS', $event.target.value)">
    </div>

    <!-- ANGLES -->
    <div class="angles">
      <div class="angles__start">
        <label class="formLabel">Start* </label>
        <input type="text"
                placeholder="(o)"
                v-model="formValues.STARTANG"
                @input="validateInput('STARTANG', $event.target.value)"/>
      </div>
      <div class="angles__end">
        <label class="formLabel">End* </label>
        <input type="text"
                placeholder="(o)"
                v-model="formValues.ENDANG"
                @input="validateInput('ENDANG', $event.target.value)">
      </div>
    </div>

    <!-- PHOTOS -->
    <div class="photos">
      <label class="formLabel">Photos* </label>
      <input class="photos__input" 
              type="text"
              placeholder="Enter number of photos"
              v-model="formValues.PHOTOS"
              @input="validateInput('PHOTOS', $event.target.value)">
    </div>

    <div class="errorContainer">
      <div class="error" v-for="(key, index) in Object.keys(errors)" :key="index">
        {{errors[key]}}
      </div>
    </div>
    
    <!-- BUTTONS -->
    <div class="buttonContainer">
      <AtomicButton v-bind="buttons.cancelButton" 
                            @click.native="cancelCircumnav"/>
      <AtomicButton v-bind="buttons.planButton" 
                            @click.native="saveCircumnav"/>

   </div>

  </div>
</template>

<script>
import GridEventBus from '@/components/Map/GridEventBus.js';
import Route from "@/data_classes/Route.js";
import Circumnavigation from "@/data_classes/Circumnavigation.js";
import AtomicButton from '@/components/atomic/AtomicButton.vue';
import { highlightSegment } from '@/components/Map/Utility/SegmentColorer.js';
import { mapGetters } from 'vuex';

export default {
  name: "EditCircumnavForm",
  components: {
    AtomicButton
  },
  props: {
    route: Route,
    circumnavigation: Circumnavigation,
    segmentIndex: Number,
  },
  data() {
    return {
      highlight: "",
      POIName: "",
      formValues: { 
        STARTANG: "",
        ENDANG: "",
        PHOTOS: "",
        RADIUS: "",
        ISCLOCKWISE: true,
        TARGETPOI: null,
      },
      errors: {
        STARTANG: "",
        ENDANG: "",
        PHOTOS: "",
        RADIUS: "",
      },
      buttons: {
        cancelButton: {
          id:'cancelSegmentButton',
          flavor:'primary',
          text:'CANCEL',
          value: 'cancelSegment',
          enabled: true,
          storeId: 'MAP'
        },
        planButton: {
          id:'planSegmentButton',
          flavor:'primary',
          text:'PLAN',
          value: 'planSegment',
          enabled: false,
          storeId: 'MAP'
        },
      }
    }
  },
  computed: {
    ...mapGetters(['targetPOI', 'editingCircumnav']),
  },
  watch: {
    // Triggered when a POI is clicked on the grid
    // when this modal is open
    targetPOI(data) {
      if (data.POICard && data.positionPx) {
        this.POIName = data.POICard.getName();
        this.formValues.TARGETPOI = data;

        this.updateGrid();
      }
    },
  },
  beforeDestroy() {
    GridEventBus.$off('RESET_EDITING_CIRCUMNAV');
  },
  destroyed() {
    // Disabling POI click listening
    this.$store.commit("setIsListeningForPOIClick", false);
    // Reset the target POI
    this.$store.commit("setTargetPOI", {POICard: null, positionPx: null});
    // Reset color
    this.highlight.removeColor();
    // Reset the editing circumnav
    this.$store.commit("setEditingCircumnav", null);
  },
  mounted() {
    // Set the values to be displayed on form based on current circumnav
    this.setFormValues();
    // Make POIs listen for clicks 
    this.$store.commit("setIsListeningForPOIClick", true);
    // Highlight segment
    this.highlight = highlightSegment();
    this.highlight.set(this.route, this.segmentIndex);

    GridEventBus.$on('RESET_EDITING_CIRCUMNAV', () => {
      this.cancelCircumnav();
    })
  },
  methods: {
    
    /**
     * Sets form values based on what is stored in circumnav data object.
     */
    setFormValues() {
      this.POIName = this.circumnavigation.POICard.getName();
      this.formValues.STARTANG = this.circumnavigation.startAngle;
      this.formValues.ENDANG = this.circumnavigation.endAngle;
      this.formValues.PHOTOS = this.circumnavigation.numPhotos;
      this.formValues.RADIUS = this.circumnavigation.radius;
      this.formValues.ISCLOCKWISE = this.circumnavigation.isClockwise;
      this.formValues.TARGETPOI = {POICard: this.circumnavigation.POICard, positionPx: this.circumnavigation.POIPosition};
    },

    /**
     * Sets the ISCLOCKWISE value in the form.
     */
    toggleDirection(key) {
      if (key === "clockwise") {
        this.formValues.ISCLOCKWISE = true;
      } else {
        this.formValues.ISCLOCKWISE = false;
      }
      this.updateGrid();
    },
    /**
     * Validates the values from input divs. 
    */
    validateInput(key, value) {
      // Validate is a number
      if (value !== "" && !this.validateIsNumber(value)) {
        this.errors[key] = key + " is not a number.";
        return;
      } 

      // Validate that angle is within normal bounds
      if ((key === "STARTANG" || key === "ENDANG") && (Number(value) > 360 || Number(value) < -360)){
        this.errors[key] = key + " must be between -360 and 360 degrees.";
        return;
      }

      this.errors[key] = "";

      this.updateGrid();
    },
    validateIsNumber(value) {
      return !isNaN(parseFloat(value)) && isFinite(value);
    },
    /**
     * Checks if the form is complete, and if so, emits the payload to the 
     * grid to preview the circumnavigation segments.
    */
    updateGrid() {
      if (this.formIsComplete()) {
        //Emit to form updates to grid
        GridEventBus.$emit('EDIT_CIRCUM_FORM_UPDATE', {route: this.route, segmentIndex: this.segmentIndex, data: this.formValues});
        
        // Enable save
        this.buttons.planButton.enabled = true;
      } 
    },
    /**
     * Checks if the form is complete.
    */
    formIsComplete() {
      let keys = Object.keys(this.formValues);
      for (let key of keys) {
        if (this.formValues[key] === null || this.formValues[key] === "") {
          this.buttons.planButton.enabled = false;
          return false;
        }
      }
      return true;
    },
    saveCircumnav() {
      // Get the new circum created from grid
      let newCircum = this.editingCircumnav;

      // Set the orignal circumnavigation obj with updated vals
      this.circumnavigation.startAngle = newCircum.startAngle;
      this.circumnavigation.endAngle = newCircum.endAngle;
      this.circumnavigation.numPhotos = newCircum.numPhotos;
      this.circumnavigation.radius = newCircum.radius;
      this.circumnavigation.isClockwise = newCircum.isClockwise;
      this.circumnavigation.POICard = newCircum.POICard;
      this.circumnavigation.POIPosition = newCircum.POIPosition;
      this.circumnavigation.waypoints = newCircum.waypoints;

      this.closeModal();
    },
    cancelCircumnav() {
      // Populate form with original circumnav
      this.setFormValues();

      // Plot the original version
      GridEventBus.$emit('EDIT_CIRCUM_FORM_UPDATE', {route: this.route, segmentIndex: this.segmentIndex, data: this.formValues});
    },
    closeModal() {
      GridEventBus.$emit('CLOSE_SEGMENT_MODAL');
    },
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_mapTab.scss';

.formLabel {
  width: 7rem;
  text-align: right;
  padding-right: 1rem;
  font-weight: 600;
}

.segmentModal {
  position: absolute;
  top: 50%;
  left: 50%;
  transform: translate(-50%, -50%);
  width: 300px;
  background-color: white;
  border-radius: 4px;
  z-index: 99;
  padding: 2rem;
  background-color: $color-background;
  box-shadow: 3px 3px 3px rgba(0, 0, 0, .28);
}

.header {
  display: flex;
  flex-direction: row;
  align-items: center;
  justify-content: center;
  margin-bottom: 2rem;

  &__title {
    font-weight: 600;
  }

  &__close {
    position: absolute;
    right: 2rem;

    &:hover {
      cursor: pointer;
      > * {
        fill: $color-primary;
      }
    }
  }
}

.direction {
  display: flex;
  flex-direction: row;
  margin-top: 2rem;

  &__container {
    display: flex;
    flex-direction: row;
  }

  &__button {
    padding: 0.5rem 1rem 0.5rem 1rem;
    background-color: $color-grey-dark;
    width: 9.5rem;
    text-align: center;
    
    &:hover {
      cursor: pointer;
    }

    &--left {
      @extend .direction__button;
      border-radius: 4px 0 0 4px;
    }

    &--right {
      @extend .direction__button;
      border-radius: 0 4px 4px 0;
    }
  }
}

.selected {
  background-color: $color-primary;
}

.divider {
  margin-top: 2rem;
  margin-bottom: 2rem;
  border-bottom: 1px solid $color-grey-dark;
}

.segName {
  display: flex;
  flex-direction: row;
}

.angles {
  margin-top: 2rem;
  display: flex;
  flex-direction: row;
  justify-content: space-between;
}

.photos, .radius {
  display: flex;
  flex-direction: row;
  margin-top: 2rem;

  &__input {
    width: 20rem;
  }
}

.errorContainer {
  margin-top: 1rem;
  margin-left: 5rem;
  display: flex;
  flex-direction: column;
  color: red;
  font-size: 1.2rem;
}

.buttonContainer {
  margin-left: 5rem;
  margin-top: 2rem;
  display: flex;
  flex-direction: row;
  justify-content: space-between;

  &__button {
    padding: 0.5rem 2rem;
    border-radius: 4px;
    border: 1px solid $color-primary;

    &:hover{ 
      cursor: pointer;
      background-color: $color-primary;
      border: none;
    }
  }
}

label {
  display: inline-block;
}

input {
  background-color: $color-grey-dark;
  border: none;
  border-radius: 5px;
  color: white;
  padding: 0.5rem;
  width: 6rem;
  text-align: center;

  &::-webkit-input-placeholder {
    color:$color-grey-light;
  }

  &::-moz-placeholder {
    color:$color-grey-light;
  }

  &::-ms-placeholder {
    color:$color-grey-light;
  }

  &::placeholder {
    color:$color-grey-light;
  }

  &:focus {
    outline: none !important;
    border:1px solid $color-primary;
    box-shadow: 0 0 5px #719ECE;
  }
}

.selected {
  background-color: $color-primary;
}

</style>
