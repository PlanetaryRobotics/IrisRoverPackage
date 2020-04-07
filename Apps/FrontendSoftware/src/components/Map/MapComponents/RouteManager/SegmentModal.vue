<template>
  <div class="segmentModal" id="segmentModal">
    <!-- HEADER -->
    <div class="header">
      <div class="header__title bold">Add SEG-01</div> <!-- TODO: UPDATE THIS -->
      <svg class="header__close" @click="closeModal()" width="12" height="13" viewBox="0 0 12 13" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M11.8866 0.822569C12.0329 0.674049 12.0329 0.43325 11.8866 0.28473C11.7402 0.13621 11.5029 0.13621 11.3566 0.28473L5.99999 5.72043L0.643436 0.284741C0.497078 0.136221 0.259785 0.136221 0.113427 0.284741C-0.0329308 0.433261 -0.0329308 0.67406 0.113427 0.82258L5.46999 6.25827L0.109768 11.6977C-0.0365894 11.8462 -0.0365896 12.087 0.109768 12.2355C0.256126 12.384 0.49342 12.384 0.639778 12.2355L5.99999 6.79611L11.3602 12.2355C11.5066 12.384 11.7439 12.384 11.8902 12.2355C12.0366 12.087 12.0366 11.8462 11.8902 11.6977L6.53 6.25827L11.8866 0.822569Z" fill="#FCFCFC"/>
      </svg>
    </div>

    <!-- TOGGLE -->
    <div class="toggle">
      <div class="toggle__label formLabel">Type</div>
      <div class="toggle__container">
        <div class="toggle__button--left" :class="{selected: show.segmentForm}" @click.capture="toggleType('segment')">
          Segment
        </div>
        <div class="toggle__button--right" :class="{selected: !show.segmentForm}" @click.capture="toggleType('circum')">
          Circumnav.
        </div>
      </div>
    </div>

    <div class="divider"/>

    <!-- SEGMENT FORM -->
    <div v-if="show.segmentForm">

      <!-- SEGMENT NAME -->
      <div class="segName">
        <div class="segName__label formLabel">Name</div>
        <div class="segName__name">SEG-01</div> <!-- TODO: UPDATE THIS -->
      </div>
      
      <!-- COORDINATES -->
      <div class="coordinates">
        <div class="coordinates__x">
          <label class="formLabel">X* </label>
          <input type="text"
                 placeholder="Enter X"
                 v-model="formValues.segment.XCOORD"
                 @input="validateInput('XCOORD', $event.target.value)"/>
        </div>
        <div class="coordinates__y">
          <label class="formLabel">Y* </label>
          <input type="text"
                 placeholder="Enter Y"
                 v-model="formValues.segment.YCOORD"
                 @input="validateInput('YCOORD', $event.target.value)">
        </div>
      </div>

      <!-- ANGLE -->
      <div class="angle">
        <label class="formLabel">Angle </label>
        <input class="angle__input" 
               type="text"
               placeholder="Enter angle to turn rover"
               v-model="formValues.segment.ANGLE"
               @input="validateInput('ANGLE', $event.target.value)">
      </div>

      <div class="errorContainer">
        <div class="error" v-for="(key, index) in Object.keys(errors)" :key="index">
          {{errors[key]}}
        </div>
      </div>
    
    </div>

    <!-- CIRCUMNAV FORM -->
    <div v-else>
      Circumnav form
    </div>

    <!-- BUTTONS -->
    <div class="buttonContainer">
      <div class="buttonContainer__button"> CANCEL </div>
      <div class="buttonContainer__button"> PLAN </div>
    </div>

  </div>
</template>

<script>
import GridEventBus from '@/components/Map/GridEventBus.js';

export default {
  name: "SegmentModal",
  data() {
    return {
      windowHeight: null,
      windowWidth: null,
      show: {
        segmentForm: true
      },
      formValues: {
        segment: {
          XCOORD: "",
          YCOORD: "",
          ANGLE: ""
        },
        circumnav: {

        }
      },
      errors: {
        XCOORD: "",
        YCOORD: "",
        ANGLE: ""
      }
    }
  },
  mounted() {
    window.onresize = function() {
      this.windowHeight = window.innerHeight;
      this.windowWidth = window.innerWidth;
    };

    addListeners();
    this.windowHeight = window.innerHeight;
    this.windowWidth = window.innerWidth;

    function addListeners(){
      document.querySelector(".segmentModal").addEventListener('mousedown', mouseDown, false);
      window.addEventListener('mouseup', mouseUp, false);
    }

    function mouseUp(){
      window.removeEventListener('mousemove', divMove, true);
    }

    function mouseDown(e){
      e.stopPropagation();
      window.addEventListener('mousemove', divMove, true);
    }

    function divMove(e){
      e.stopPropagation();
      var div = document.querySelector(".segmentModal");
      div.style.position = 'absolute';

      function calculateValue(client, boundary) {
        let val = client;
        if (client < 0) {
          val = 0;
        } else if (client > boundary) {
          val = boundary;
        }
        return val;
      }

      let x = calculateValue(e.clientX, this.windowWidth);
      let y = calculateValue(e.clientY, this.windowHeight);

      div.style.top = y + 'px';
      div.style.left = x + 'px';
    }
  },
  computed: {

  },
  methods: {
    validateInput(key, value) {
      // Validate is a number
      if (value !== "" && !this.validateIsNumber(value)) {
        this.errors[key] = key + " is not a number.";
        return;
      } 

      // Check for non-angle vals, cannot be negative
      if (key !== "ANGLE" && Number(value) < 0) {
        this.errors[key] = key + " cannot be a negative number.";
        return;
      }

      this.errors[key] = "";
    },
    validateIsNumber(value) {
      return !isNaN(parseFloat(value)) && isFinite(value);
    },
    toggleType(type) {
      if (type === 'segment' && !this.show.segmentForm ||
          type === 'circum' && this.show.segmentForm) {
        this.show.segmentForm = !this.show.segmentForm;
      }
    },
    closeModal() {
      GridEventBus.$emit('CLOSE_ADD_MODAL');
    }
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
