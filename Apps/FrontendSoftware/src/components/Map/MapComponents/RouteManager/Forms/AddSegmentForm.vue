<template>
  <div>
    <!-- SEGMENT NAME -->
    <div class="segName">
      <div class="segName__label formLabel">Name</div>
      <div class="segName__name">{{segmentName}}</div>
    </div>
    
    <!-- COORDINATES -->
    <div class="coordinates">
      <div class="coordinates__x">
        <label class="formLabel">X* </label>
        <input type="text"
                placeholder="Enter X"
                v-model="formValues.XCOORD"
                @input="validateInput('XCOORD', $event.target.value)"/>
      </div>
      <div class="coordinates__y">
        <label class="formLabel">Y* </label>
        <input type="text"
                placeholder="Enter Y"
                v-model="formValues.YCOORD"
                @input="validateInput('YCOORD', $event.target.value)">
      </div>
    </div>

    <!-- ANGLE -->
    <div class="angle">
      <label class="formLabel">Angle </label>
      <input class="angle__input" 
              type="text"
              placeholder="Enter angle to turn rover"
              v-model="formValues.ANGLE"
              @input="validateInput('ANGLE', $event.target.value)">
    </div>

    <div class="errorContainer">
      <div class="error" v-for="(key, index) in Object.keys(errors)" :key="index">
        {{errors[key]}}
      </div>
    </div>
    
    <!-- BUTTONS -->
    <div class="buttonContainer">
      <AtomicButton v-bind="buttons.cancelButton" 
                            @click.native="cancelSegment"/>
      <AtomicButton v-bind="buttons.planButton" 
                            @click.native="saveSegment"/>

   </div>

  </div>
</template>

<script>
import GridEventBus from '@/components/Map/GridEventBus.js';
import Route from "@/data_classes/Route.js";
import AtomicButton from '@/components/atomic/AtomicButton.vue';
import { mapGetters } from 'vuex';

export default {
  name: "AddSegmentForm",
  components: {
    AtomicButton
  },
  props: {
    route: Route
  },
  data() {
    return {
      formValues: { 
        XCOORD: "",
        YCOORD: "",
        ANGLE: ""
      },
      errors: {
        XCOORD: "",
        YCOORD: "",
        ANGLE: ""
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
    ...mapGetters(['currWaypointSegment']),
    segmentName: function() {
      let currLength = this.route.segmentList.length;
      return "SEG-" + (currLength);
    }
  },
  watch: {
    // Waypoint segment will be saved into store from grid
    // and grid ensures that segment has all coords before saving.
    // So whenever seg updates, it is ready to save.
    currWaypointSegment(newVal) {
      if (newVal !== null) {
        this.buttons.planButton.enabled = true;
      } else {
        this.buttons.planButton.enabled = false;
      }
    }
  },
  destroyed() {
    this.$store.commit("setCurrWaypointSegment", null);
    this.$store.commit("setIsListeningForNewWaypoint", false);
    this.$store.commit("setEditingRoute", null);
  },
  mounted() {
    this.$store.commit("setIsListeningForNewWaypoint", true);
    this.$store.commit("setEditingRoute", this.route);

    GridEventBus.$on('WAYPOINT_GRID_UPDATE', (data) => {
      this.updateFormValues(data.xCm, data.yCm);
    })
  },
  methods: {
    updateFormValues(xCm, yCm) {
      this.formValues.XCOORD = Math.round(xCm);
      this.formValues.YCOORD = Math.round(yCm);
    },
    validateInput(key, value) {
      // Validate is a number
      if (value !== "" && !this.validateIsNumber(value)) {
        this.errors[key] = key + " is not a number.";
        return;
      } 

      // Validate that angle is within normal bounds
      if (key === "ANGLE" && (Number(value) > 360 || Number(value) < -360)){
        this.errors[key] = key + " must be between -360 and 360 degrees.";
        return;
      }

      this.errors[key] = "";

      // Emit to form updates to grid
      GridEventBus.$emit('ADD_SEG_FORM_UPDATE', {xCm: this.formValues.XCOORD, 
                                                  yCm: this.formValues.YCOORD,
                                                  angle: this.formValues.ANGLE});
    },
    validateIsNumber(value) {
      return !isNaN(parseFloat(value)) && isFinite(value);
    },
    saveSegment() {
      this.$store.commit("saveSegment", {route: this.route, segment: this.currWaypointSegment});
      this.closeModal();
    },
    cancelSegment() {
      this.$store.commit("setCurrWaypointSegment", null);
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
  width: 5rem;
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

.toggle {
  display: flex;
  flex-direction: row;
  // align-items: center;
  // justify-content: center;

  &__container {
    display: flex;
    flex-direction: row;
  }

  &__button {
    padding: 0.5rem 1rem 0.5rem 1rem;
    background-color: $color-grey-dark;
    width: 10.5rem;
    text-align: center;
    
    &:hover {
      cursor: pointer;
    }

    &--left {
      @extend .toggle__button;
      border-radius: 4px 0 0 4px;
    }

    &--right {
      @extend .toggle__button;
      border-radius: 0 4px 4px 0;
    }
  }
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

.coordinates {
  margin-top: 2rem;
  display: flex;
  flex-direction: row;
  justify-content: space-between;
}

.angle {
  display: flex;
  flex-direction: row;
  margin-top: 2rem;

  &__input {
    width: 22rem;
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
