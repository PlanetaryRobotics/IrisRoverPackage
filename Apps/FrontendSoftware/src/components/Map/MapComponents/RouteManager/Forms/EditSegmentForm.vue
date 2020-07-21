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
import WaypointSegment from "@/data_classes/WaypointSegment.js";
import AtomicButton from '@/components/atomic/AtomicButton.vue';
import { highlightSegment } from '@/components/Map/Utility/SegmentColorer.js';
// import { mapGetters } from 'vuex';

export default {
  name: "EditSegmentForm",
  components: {
    AtomicButton
  },
  props: {
    route: Route,
    segment: WaypointSegment,
    segmentIndex: Number,
  },
  data() {
    return {
      highlight: "",
      originalSegment: "",
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
      },
      state: null,
    }
  },
  computed: {
    segmentName: function() {
      return "SEG-" + this.segmentIndex;
    }
  },
  watch: {
    // This happens when another edit modal is opened while the current is opened
    segment() {
      // Set the new state
      this.state.setNewState(this.route, this.segment, this.segmentIndex);

      // Make sure to revert the previous since was not saved
      let oldRoute = this.state.getPrevState().route;
      let oldSegment = this.state.getPrevState().segment;
      let oldIdx = this.state.getPrevState().segmentIndex;
      GridEventBus.$emit('EDIT_SEG_FORM_UPDATE', {route: oldRoute, 
                                                  segmentIndex: oldIdx, 
                                                  xCm: oldSegment.xCmCoordinate, 
                                                  yCm: oldSegment.yCmCoordinate, 
                                                  roverAngle: oldSegment.roverAngle});
      
      // Set the values to be displayed on form based on current new segment
      this.setFormValues();
      // Highlight it
      this.highlight.set(this.route, this.segmentIndex);
      // Update store so grid knows which seg we are editing
      this.$store.commit("setEditingSegmentInfo", {route: this.route, segmentIdx: this.segmentIndex});
    }
  },
  destroyed() { 
    // Make grid stop listening for clicks
    this.$store.commit("setIsListeningForEditWaypoint", false);
    // Update store so grid knows we are not looking at any segment to edit now
    this.$store.commit("setEditingSegmentInfo", {route: null, segmentIdx: null});
    // Reset color
    this.highlight.removeColor();
  },
  mounted() {
    // Set the values to be displayed on form based on current new segment
    this.setFormValues();
    // Highlight segment
    this.highlight = highlightSegment();
    this.highlight.set(this.route, this.segmentIndex);

    // Update store so grid knows to listen + knows which seg we are editing
    this.$store.commit("setIsListeningForEditWaypoint", true);
    this.$store.commit("setEditingSegmentInfo", {route: this.route, segmentIdx: this.segmentIndex});

    // Init a state tracker (in event user clicks to edit another seg while modal is open)
    let method = this.stateTracker;
    this.state = new method({route: this.route, segment: this.segment, segmentIndex: this.segmentIndex});

    // Listens for when there is grid clicked
    GridEventBus.$on('WAYPOINT_GRID_UPDATE', (data) => {
      this.updateFormValues(data.xCm, data.yCm);
      this.buttons.planButton.enabled = true;
    })
  },
  methods: {

    /**
     * Helper function obj used to track the current segment being edited, and any previous segment. 
     * 
     * @param {object} state in format {route: route, segment: segment, segmentIndex: index}
     */
    stateTracker(state) {
      let prevState = null;
      let currState = state;
      return {
        setNewState: function(route, segment, idx) {
          prevState = currState;
          currState = {route: route, segment: segment, segmentIndex: idx};
        },
        getPrevState: function() {
          return prevState;
        }
      }
    },

    /**
     * Sets the form values based on what is stored in the segment object.
     */
    setFormValues() {
      this.originalSegment = this.segment;
      this.formValues.XCOORD = Math.round(this.segment.xCmCoordinate);
      this.formValues.YCOORD = Math.round(this.segment.yCmCoordinate);
      this.formValues.ANGLE = this.segment.roverAngle;
    },

    /**
     * Updates form values based on input.
     */
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

      if (value === "") {
        this.formValues.key = 0;
      }

      this.errors[key] = "";

      GridEventBus.$emit('EDIT_SEG_FORM_UPDATE', {route: this.route, segmentIndex: this.segmentIndex, xCm: this.formValues.XCOORD, yCm: this.formValues.YCOORD, roverAngle: this.formValues.ANGLE});
    
      this.buttons.planButton.enabled = true; // At this point, can technically save
    },
    validateIsNumber(value) {
      return !isNaN(parseFloat(value)) && isFinite(value);
    },
    saveSegment() {
      // Save the CM coordinates and angle into object
      this.segment.xCmCoordinate = this.formValues.XCOORD;
      this.segment.yCmCoordinate = this.formValues.YCOORD;
      this.segment.roverAngle = this.formValues.ANGLE;

      // Make the PX version of coords are computed too
      GridEventBus.$emit('COMPUTE_SEG_PX_COORDS', this.segment);
      this.closeModal();
    },
    cancelSegment() {
      let obj = {
        XCOORD: Math.round(this.segment.xCmCoordinate),
        YCOORD: Math.round(this.segment.yCmCoordinate),
        ANGLE: this.segment.roverAngle
      }
      this.formValues = obj;
      GridEventBus.$emit('EDIT_SEG_FORM_UPDATE', {route: this.route, segmentIndex: this.segmentIndex, xCm: this.formValues.XCOORD, yCm: this.formValues.YCOORD, roverAngle: this.formValues.roverAngle});
      this.buttons.planButton.enabled = false; // Reverting to original, so disable again
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
