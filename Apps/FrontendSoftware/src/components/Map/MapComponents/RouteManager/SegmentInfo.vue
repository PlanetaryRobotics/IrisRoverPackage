<template>
  <div class="segmentInfo">

    <div class="segment__header">
      <!-- ARROW -->
      <svg @click="toggleCommands" width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="icon arrow" :class="{ open : show.commands }">
        <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
      </svg>

      <!-- SEG ICON -->
      <svg width="12" height="12" viewBox="0 0 12 12" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path d="M10.47 5.5C10.24 3.415 8.585 1.76 6.5 1.53V0.5H5.5V1.53C3.415 1.76 1.76 3.415 1.53 5.5H0.5V6.5H1.53C1.76 8.585 3.415 10.24 5.5 10.47V11.5H6.5V10.47C8.585 10.24 10.24 8.585 10.47 6.5H11.5V5.5H10.47ZM6 9.5C4.065 9.5 2.5 7.935 2.5 6C2.5 4.065 4.065 2.5 6 2.5C7.935 2.5 9.5 4.065 9.5 6C9.5 7.935 7.935 9.5 6 9.5Z" fill="white"/>
      </svg>

      <div class="segment__name">
        {{'SEG-' + index}}
      </div>

      <!-- EDIT ICON -->
      <svg @click="openEditModal()" class="icon pencil" width="12" height="12" viewBox="0 0 12 12" fill="none" xmlns="http://www.w3.org/2000/svg">
        <path fill-rule="evenodd" clip-rule="evenodd" d="M8.84607 2.54393C8.78749 2.48536 8.69251 2.48536 8.63393 2.54393L7.82295 3.35492C7.76437 3.41349 7.76437 3.50847 7.82295 3.56705L8.43727 4.18136C8.69111 4.4352 8.69111 4.84676 8.43727 5.1006L3.60678 9.93109C3.55989 9.97798 3.4963 10.0043 3.43 10.0043H2V8.57431C2 8.50801 2.02634 8.44442 2.07322 8.39754L6.39989 4.07087C6.49752 3.97324 6.65581 3.97324 6.75344 4.07087C6.85107 4.1685 6.85107 4.32679 6.75344 4.42442L2.5 8.67787V9.50431H3.32645L8.08371 4.74705C8.14229 4.68847 8.14229 4.59349 8.08371 4.53491L7.4694 3.9206C7.21556 3.66676 7.21556 3.2552 7.4694 3.00136L8.28038 2.19038C8.53422 1.93654 8.94578 1.93654 9.19962 2.19038L9.81393 2.8047C10.0678 3.05854 10.0678 3.47009 9.81393 3.72393L9.50678 4.03109C9.40915 4.12872 9.25085 4.12872 9.15322 4.03109C9.05559 3.93346 9.05559 3.77517 9.15322 3.67754L9.46038 3.37038C9.51896 3.3118 9.51896 3.21683 9.46038 3.15825L8.84607 2.54393Z" fill="#FCFCFC"/>
      </svg>
    </div>

    <!-- COORD LIST -->
    <div class = "segment__attr">
      <div>{{"(" + Math.round(segment.xCmCoordinate) + "cm, " +  Math.round(segment.yCmCoordinate) + "cm)"}}</div>
    </div>

    <!-- COMMANDS -->
    <div v-show="show.commands" class ="commands">
      <div class="commands__pill">
        TurnRight (10°, 15°/s)
      </div>
      <div class="commands__pill">
        MoveForward (145cm, 15cm/s)
      </div>
    </div>
  </div>
</template>

<script>

import GridEventBus from '@/components/Map/GridEventBus.js';
import WaypointSegment from "@/data_classes/WaypointSegment.js";
import Route from "@/data_classes/Route.js";

export default {
  name: "SegmentInfo",
  props: {
    segment: WaypointSegment,
    route: Route,
    index: Number,
  },
  data() {
    return {
      show: {
        commands: false
      },
    }
  },
  methods: {
    toggleCommands() {
      this.show.commands = !this.show.commands;
    },
    openEditModal() {
       GridEventBus.$emit("OPEN_SEGMENT_MODAL", {route: this.route, segment: this.segment, segmentIndex: this.index, action: "EDIT"});
    },
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

.segment {
  padding-bottom: 1rem;

  &__header {
    display: flex;
    flex-direction: row;
    align-items: center;
    justify-content: flex-start;
  }

  &__name {
    padding-left: 1rem;
    flex-grow: 1;
  }

  &__attr {
    margin-top: 0.5rem;
    margin-left: 4.5rem;
  }
}

.icon {
  width: 1.5rem;
  height: 1.5rem;
  transform: rotate(-180deg);
  transition: .1s ease-in-out;
}

.arrow {
  stroke: $color-grey;
  margin-right: 1rem;
  &:hover {
    cursor: pointer;
    stroke: $color-primary;
  }
}

.pencil {
  > * {
    fill: white;
  }

  &:hover {
    cursor: pointer;
    > * {
      fill: $color-primary;
    }
  }
}

.commands {
  margin-top: 1rem;
  margin-left: 2rem;
  &__pill {
    display: inline-block;
    background-color: #424242;
    padding: 0.5rem 2rem 0.5rem 2rem;
    color: white;
    border-radius: 15px;
    margin-bottom: 0.5rem;
  }
}

.open {
  transform: rotate(0deg);
}

</style>
