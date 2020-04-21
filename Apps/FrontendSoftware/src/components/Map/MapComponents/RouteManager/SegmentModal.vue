<template>
  <div class="segmentModal" id="segmentModal">
    <!-- HEADER -->
    <div class="header">
      <div class="header__title bold">{{action.charAt(0) + action.substring(1).toLowerCase() + " " + getSegmentName()}}</div> <!-- TODO: UPDATE THIS -->
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

    <!-- DIVIDER -->
    <div class="divider"/>

    <!-- ADD SEGMENT FORM -->
    <div v-if="show.segmentForm">
      <AddSegmentForm :route = "route" v-if="action === 'ADD'"/>
      <EditSegmentForm :route = "route" :segment = "segment" :segmentIndex = "segmentIndex" v-else/>
    </div>

    <!-- CIRCUMNAV FORM -->
    <div v-if="!show.segmentForm">
      Circumnav form
    </div>

  </div>
</template>

<script>
import GridEventBus from '@/components/Map/GridEventBus.js';
import Route from "@/data_classes/Route.js";
import AddSegmentForm from "@/components/Map/MapComponents/RouteManager/Forms/AddSegmentForm.vue";
import EditSegmentForm from "@/components/Map/MapComponents/RouteManager/Forms/EditSegmentForm.vue";
import WaypointSegment from "@/data_classes/WaypointSegment.js";

export default {
  name: "SegmentModal",
  components: {
    AddSegmentForm,
    EditSegmentForm,
  },
  props: {
    route: Route,
    action: {
      validator: function (value) {
        return ['ADD', 'EDIT'].indexOf(value) !== -1;
      }
    },
    segment: {
      validator: function (value) {
        // When it is an ADD modal
        if (!value || value === "") { return true; }

        // When it is an EDIT modal
        if (value instanceof WaypointSegment) {return true; }
        return false;
      }
    },
    segmentIndex: {
      validator: function(value) {
        if (!value || value === "") {return true;}

        if (typeof value === "number") {
          return true;
        }

        return false;
      }
    }
  },
  data() {
    return {
      windowHeight: null,
      windowWidth: null,
      show: {
        segmentForm: true
      },
    }
  },
  mounted() {
    this.setupModalPositioning();
  },
  methods: {
    getSegmentName: function() {
      if (this.action === "ADD") {
        let currLength = this.route.segmentList.length;
        return "SEG-" + (currLength);
      } else {
        return "SEG-" + this.segmentIndex;
      }
    },
    isAdd() {
      return this.action === "ADD";
    },
    closeModal() {
      GridEventBus.$emit('CLOSE_SEGMENT_MODAL');
    },
    toggleType(type) {
      if (type === 'segment' && !this.show.segmentForm ||
          type === 'circum' && this.show.segmentForm) {
        this.show.segmentForm = !this.show.segmentForm;
      }
    },
    setupModalPositioning() {
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
