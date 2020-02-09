<template>
  <div id = "createRoute">
    <div class="mapTab">

      <!-- HEADER --> 
      <div class="mapTab__header" @click="toggleTab">
        <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="mapTab__icon" :class="{ open : show.createTab }">
        <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
        </svg>
        <h2 class="text__main--bold mapTab__title">Create Route</h2>
      </div>

      <!-- CONTENT --> 
      <div class="mapTab__content" v-show = "show.createTab">
        <div class="content">
          <span class="tabContainer">
            <button class="tabButton left"
                    :class="{selected: 'absolute' == selectedTab}"
                    @click="setSelectedTab('absolute')"
                    >
              Absolute[A]
            </button>
            <button class="tabButton right"
                    :class="{selected: 'relative' == selectedTab}"
                    @click="setSelectedTab('relative')"
                    >
              Relative[R]
            </button>
          </span>

          <!-- FORM FOR RELATIVE SEGMENT -->
          <div v-if="selectedTab === 'relative'">

            <div class="inputBlock">
              <label>Route Name: </label>
              <input type="text" 
                    placeholder="Enter name of route"
                    v-model="relativeFormInputs.routeName" 
                    @input="updateRelativeSegment('routeName', $event.target.value)"
                    />
            </div>
            <div class="errorBlock">
              {{show.errorMessages.routeName ? errorMessages.routeName : ""}}
            </div>

            <div class="inputBlock">
              <label>Distance: </label>
              <input type="text" 
                    placeholder="Enter distance in cm"
                    v-model="relativeFormInputs.distance"
                    @input="updateRelativeSegment('distance', $event.target.value)"/>
            </div>
            <div class="errorBlock">
              {{show.errorMessages.distance ? errorMessages.distance : ""}}
            </div>

            <div class="inputBlock">
              <label>Angle: </label>
              <input type="text" 
                    placeholder="Enter angle in degrees"
                    v-model="relativeFormInputs.angle"
                    @input="updateRelativeSegment('angle', $event.target.value)"/>
            </div>
            <div class="errorBlock">
              {{show.errorMessages.angle ? errorMessages.angle : ""}}
            </div>
          </div>

          <!-- FORM FOR ABSOLUTE SEGMENT -->
          <div v-else>
            <div class="inputBlock">
              <label>Route Name: </label>
              <input type="text" 
                    placeholder="Enter name of route"
                    v-model="absoluteFormInputs.routeName" 
                    @input="updateAbsoluteSegment('routeName', $event.target.value)"
                    />
            </div>
            <div class="errorBlock">
              {{show.errorMessages.routeName ? errorMessages.routeName : ""}}
            </div>

            <div class="inputBlock">
              <label>X Coordinate: </label>
              <input type="text" 
                    placeholder="Enter x coordinate in cm"
                    v-model="absoluteFormInputs.xCoord"
                    @input="updateAbsoluteSegment('xCoord', $event.target.value)"/>
            </div>
            <div class="errorBlock">
              {{show.errorMessages.xCoord ? errorMessages.xCoord : ""}}
            </div>

            <div class="inputBlock">
              <label>Y Coordinate: </label>
              <input type="text" 
                    placeholder="Enter y coordinate in cm"
                    v-model="absoluteFormInputs.yCoord"
                    @input="updateAbsoluteSegment('yCoord', $event.target.value)"/>
            </div>
            <div class="errorBlock">
              {{show.errorMessages.yCoord ? errorMessages.yCoord : ""}}
            </div>
          </div>

          <!-- CANCEL/PLAN BUTTON -->
          <span class="buttonContainer">
            <AtomicButton v-bind="cancelRouteButton" 
                          @click.native="cancelRoute"/>
            <AtomicButton v-bind="planRouteButton" 
                          @click.native="saveRoute"/>
          </span>
        </div>
      </div>

    <!-- END EDIT TAB --> 
    </div>
  </div>
</template>

<script>

import $ from 'jquery'
import AtomicButton from '@/components/atomic/AtomicButton.vue';

export default {
  name: "CreateRoute",
  components: {
    AtomicButton
  },
  data() {
    return {
      selectedTab: "absolute",
      show: {
        createTab: false,
        errorMessages: {
          routeName: false,
          angle: false,
          distance: false,
          xCoord: false,
          yCoord: false
        }
      },
      errorMessages: {
        routeName: "",
        distance: "",
        angle: "",
        xCoord: "",
        yCoord: ""
      },
      cancelRouteButton: {
        id:'cancelRouteButton',
        flavor:'primary',
        text:'CANCEL',
        value: 'cancelRoute',
        enabled: true,
        storeId: 'MAP'
      },
      planRouteButton: {
        id:'planRouteButton',
        flavor:'primary',
        text:'PLAN',
        value:'planRoute',
        enabled: false,
        storeId: 'MAP'
      },
      relativeFormInputs: {
        routeName: "",
        distance: "",
        angle: "",
      },
      absoluteFormInputs: {
        routeName: "",
        xCoord: "",
        yCoord: "",
      },
    }
  },
  methods: {
    /**
     * Will show/hide the addEditTab on click.
    */
    toggleTab() {
      this.show.createTab = !this.show.createTab;
    },

    /**
     * Method triggered when a tab is clicked.
     * Will:
     *   - Reset all form inputs
     *   - Update selectedTab in data()
     *   - Update value in store
     * 
     * @param {string} tabName      name of tab, must be "absolute" or "relative"
    */
    setSelectedTab(tabName) {

      if (tabName !== "absolute" && tabName !== "relative") {
        throw new Error("tabName is not valid. Must be either 'absolute' or 'relative'.");
      }

      // Reset all form inputs on switch
      this.resetForm();

      // Update data
      this.selectedTab = tabName;

      // Remove what is currently plotted
      this.$store.commit("triggerCurrSegmentRemoval");
    },

    updateAbsoluteSegment: function(param, value) {

      if (param === "xCoord" || param === "yCoord") {
        if (this.validationCheckNumber(value)) {
          this.show.errorMessages[param] = false;
        }
        else if (value === "-") {
          return
        }
        else if (value === "") {
          this.show.errorMessages[param] = false;
        }
        else {
          this.errorMessages[param] = "Coordinate must be a number.";
          this.show.errorMessages[param] = true;
          this.planRouteButton.enabled = false;
          return;
        }
      }

      if (param === "routeName") {
        this.absoluteFormInputs[param] = value;
      }

      this.absoluteFormInputs.routeType = this.selectedTab;

      // Commit updated currRoute to store
      this.$store.commit("updateCurrSegment", this.absoluteFormInputs);

      // Enables PLAN button if route has all necessary params
      if (this.isSegmentComplete()) {
        this.planRouteButton.enabled = true;
      } else {
        this.planRouteButton.enabled = false;
      }
    },

    /**
     * Performs validation check on the route param 
     * updated from form input.
     * 
     * If valid:
     *   - Sets it to the data(), 
     *   - Commits route object to store,
     *   - Checks if route object is complete to activate PLAN button
     * 
     * @param {string} param      name of route param
     *                            must be "distance", "angle" or "routeName"
     * @param {string} value      value of the param
    */
    updateRelativeSegment: function(param, value) {
      
      // Validation check on distance and angle
      if (param === "distance" || param === "angle") {

        // Distance cannot be negative
        if (param === "distance" && value === "-") {
          this.errorMessages["distance"] = "Distance must be a positive number.";
          this.show.errorMessages["distance"] = true;
          this.planRouteButton.enabled = false;
          return;
        }
        // Check is valid number (and can be pos/neg if is angle)
        else if (this.validationCheckNumber(value)) {
          this.show.errorMessages[param] = false;
        }
        else if (value === "-") {
          return
        }
        else if (value === "") {
          this.show.errorMessages[param] = false;
        } else {
          let upper = param.charAt(0).toUpperCase() + param.substring(1);
          this.errorMessages[param] = upper + " must be a number.";
          this.show.errorMessages[param] = true;
          this.planRouteButton.enabled = false;
          return;
        }
      }

      // Update data()
      if ((param === "distance" || param === "angle") && value !== "-" && value !== "") {
        this.relativeFormInputs[param] = parseInt(value);
      } else {
        this.relativeFormInputs[param] = value;
      }

      if (param === "routeName") {
        this.relativeFormInputs[param] = value;
      }

      this.relativeFormInputs.routeType = this.selectedTab;

      // Commit updated currRoute to store
      this.$store.commit("updateCurrSegment", this.relativeFormInputs);

      // Enables PLAN button if route has all necessary params
      if (this.isSegmentComplete()) {
        this.planRouteButton.enabled = true;
      } else {
        this.planRouteButton.enabled = false;
      }
    },

    /**
     * Checks if string is a valid number (positive or negative).
     * 
     * @param {string}  value
     * @return {boolean}
    */
    validationCheckNumber(value) {
      return /^\d+$/.test(value) || value < 0;
    },
    
     /**
     * Checks if form is complete.
     * 
     * @return {boolean}
    */
    isSegmentComplete() {
      let form;
      if (this.selectedTab === "relative") {
        form = this.relativeFormInputs;
      } else {
        form = this.absoluteFormInputs;
      }

      let keys = Object.keys(form);
      for (let key of keys) {
        if (form[key] === "") {
          return false;
        }
      }

      return true;
    },

    saveRoute() {
      if (this.selectedTab === "relative") {
        this.$store.commit("saveCurrRoute", this.relativeFormInputs.routeName);
      } else {
        this.$store.commit("saveCurrRoute", this.absoluteFormInputs.routeName);
      }
      this.resetForm();
      this.planRouteButton.enabled = false;
    },

    cancelRoute() {
      this.resetForm();
      this.planRouteButton.enabled = false;
      this.$store.commit("triggerCurrSegmentRemoval");
    },

    resetForm() {
      let keys = this.selectedTab === "relative" ? Object.keys(this.relativeFormInputs) : Object.keys(this.absoluteFormInputs);
      for (let key of keys) {
        if (this.selectedTab === "relative") {
          this.relativeFormInputs[key] = "";
        } else {
          this.absoluteFormInputs[key] = "";
        }
        this.show.errorMessages[key] = false;
        this.errorMessages[key] = "";
      }
    }
  },
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_mapTab.scss';

$left-offset: 14px; 

#createRoute {
  background-color: $color-background;
}
.content {
  padding-top: 2rem;

  &__item {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding-bottom: 1rem;

    &--visibility {
      display: flex;
      align-items: center;
    }
  }

}

.content {
  margin-left: $left-offset;
}

.buttonContainer, .tabContainer {
  display: flex;
  justify-content: left;
  padding-left: 8rem;
  padding-bottom: 3rem;
}

.buttonContainer {
  > div {
    padding-right: 2rem;
  }
}

.tabButton {
  background-color: $color-grey-dark;
  color: white;
  padding: 0.5rem 1.5rem;
  border: none;

  &.selected {
    background-color: $color-primary;
  }

  &.left {
    border-radius: 5px 0 0 5px;
  }

  &.right {
    border-radius: 0 5px 5px 0;
  }
}

.inputBlock {
  display:flex;
  flex-direction:row;

  label {
    width: 8rem;
    padding-right: 1rem;
    align-self: center;
    color: white;
    font-weight: bold;
  }

  input {
    flex:1;
    background-color: $color-grey-dark;
    border: none;
    border-radius: 5px;
    color: white;
    padding: 1rem;

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
}

.errorBlock {
  color: red;
  padding-left: 8rem;
  height: 2rem;
  margin-bottom: 0.5rem;
}

</style>
