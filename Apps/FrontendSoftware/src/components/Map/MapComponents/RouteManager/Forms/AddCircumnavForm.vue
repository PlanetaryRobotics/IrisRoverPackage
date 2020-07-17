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
import AtomicButton from '@/components/atomic/AtomicButton.vue';
import { mapGetters } from 'vuex';

export default {
  name: "AddCircumnavForm",
  components: {
    AtomicButton
  },
  props: {
    route: Route
  },
  data() {
    return {
      POIName: "Click POI on grid",
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
    ...mapGetters(['targetPOI']),
  },
  watch: {
    // Triggered when a POI is clicked on the grid
    // when this modal is open
    targetPOI(data) {
      this.POIName = data.POICard.getName();
      this.formValues.TARGETPOI = data;

      this.activateGridPreview();
    }
  },
  destroyed() {
    // Disabling POI click listening
    this.$store.commit("setIsListeningForPOIClick", false);
    // Reset the target POI
    this.$store.commit("setTargetPOI", {POICard: null, positionPx: null});
  },
  mounted() {
    // Make POIs listen for clicks 
    this.$store.commit("setIsListeningForPOIClick", true);
  },
  methods: {
    /**
     * Sets the ISCLOCKWISE value in the form.
    */
    toggleDirection(key) {
      if (key === "clockwise") {
        this.formValues.ISCLOCKWISE = true;
      } else {
        this.formValues.ISCLOCKWISE = false;
      }
      this.activateGridPreview();
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

      this.activateGridPreview();
    },
    validateIsNumber(value) {
      return !isNaN(parseFloat(value)) && isFinite(value);
    },
    /**
     * Checks if the form is complete, and if so, emits the payload to the 
     * grid to preview the circumnavigation segments.
    */
    activateGridPreview() {
      if (this.formIsComplete()) {
        console.log("COMPLETE!");

        //Emit to form updates to grid
        GridEventBus.$emit('ADD_CIRCUM_FORM_UPDATE', this.formValues);
      } else {
        console.log("NOT COMPLETE!");
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

      this.buttons.planButton.enabled = true;
      return true;
    },
    saveCircumnav() {
      // this.$store.commit("saveSegment", {route: this.route, segment: this.currWaypointSegment});
      this.closeModal();
    },
    cancelCircumnav() {
      // this.$store.commit("triggerCurrSegmentRemoval");
      let keys = Object.keys(this.formValues);
      for (let k of keys) {
        this.formValues[k] = "";
      }
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
