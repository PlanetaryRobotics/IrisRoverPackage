<template>
  <div class="routeEntry">
    <div class="routeTab">
      <!-- DOWN ARROW -->
      <div class="routeTab__header" @click="toggleSegmentList">
        <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="routeTab__icon" :class="{ open : show.segmentList }">
        <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
        </svg>
      </div>

      <!-- ROUTE NAME --> 
      <p class="route__item--name text__main"> {{route.routeName}} </p>

      <!-- VISIBILITY ICON -->
      <div class="route__item--visibility" @click="toggleVisibility(route)" >
        <svg width="19" height="14" viewBox="0 0 19 14" fill="none" xmlns="http://www.w3.org/2000/svg" class="routeIcon" :class="{selected: route.isVisible}" >
          <circle cx="9.50019" cy="6.88459" r="2.76923"/>
          <path d="M9.5 1C4.92308 1 1 4.92308 1 6.88462C1 8.84615 4.92308 12.7692 9.5 12.7692C14.0769 12.7692 18 8.84615 18 6.88462C18 4.92308 14.0769 1 9.5 1Z"/>
        </svg>
      </div>
    </div>
  
    <!-- SEGMENT LIST -->
    <div class="segmentList" v-show = "show.segmentList">
      <div class="segment" v-for="(segment, index) in route.segmentList" :key="index">

        <!-- SEGMENT HEADER -->
        <div class="segment__header">
          <svg width="10" height="9" viewBox="0 0 10 9" fill="none" xmlns="http://www.w3.org/2000/svg">
            <line x1="8.75" x2="8.75" y2="9" stroke="#C4C4C4" stroke-width="1.5"/>
            <line y1="8.25" x2="9" y2="8.25" stroke="#C4C4C4" stroke-width="1.5"/>
          </svg>
          <div class="segment__name">
            {{'SEG-' + index}}
          </div>
        </div>
        
        <!-- SEGMENT ATTR -->
        <div class = "segment__attr" v-if = "segment.constructor.name === 'RelativeSegment'">
          <div>{{"[R] " + segment.distance + " cm"}}</div>
          <div>{{"[R] " + segment.angle + " &#176;"}}</div>
          <div>{{"[R] [" + segment.xCoordinate + "cm, " +  segment.yCoordinate + "cm]"}}</div>
        </div>
        <div class = "segment__attr" v-else>
          <div>{{"[A] [" + segment.xCoordinate + "cm, " +  segment.yCoordinate + "cm]"}}</div>
        </div>
      </div>

    </div>

  </div>    
  
</template>

<script>

import $ from 'jquery';
import { mapMutations, mapGetters } from 'vuex';

export default {
  name: "RouteEntry",
  props: {
    route: Object
  },
  data() {
    return {
      show: {
        segmentList: false,
        route: false,
      },
    }
  },
  computed: {
    ...mapGetters({
      routeList: 'routeList',
    }),
  },
  methods: {
    ...mapMutations({
      triggerRouteListUpdate: 'triggerRouteListUpdate'
    }),
    toggleSegmentList() {
      this.show.segmentList = !this.show.segmentList;
    },
    toggleVisibility(route) {
      route.isVisible = !route.isVisible;
      this.triggerRouteListUpdate();
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

.segmentList {
  padding-top: 2rem;
  padding-left: 3rem;
}

.segment {
  padding-bottom: 1rem;

  &__header {
    display: flex;
    flex-direction: row;
    align-items: center;
  }

  &__name {
    padding-left: 1rem;
    font-weight: bold;
  }

  &__attr {
    >div {
      padding: 1rem 0 0 2rem;
    }
  }
}

.routeTab {
  display: flex;
  flex-direction: row;
  justify-content: flex-start;

  &__header {
    display: flex;
    align-items: center;
    cursor: pointer;
    user-select: none;
    padding-left: 1rem;
  }

  &__icon {
    stroke: $color-grey-light;
    width: 14px;
    height: 14px;
    transform: rotate(-90deg);
    transition: .1s ease-in-out;
  }

  &__name {
    padding-left: 0.5rem;
    flex-grow: 1;
  }
}

.route {
  &__item {
    display: flex;
    align-items: center;
    justify-content: space-between;

    &--name {
      padding-left: 0.5rem;
      flex-grow: 1;
    }

    &--visibility {
      display: flex;
      align-items: center;
    }
  }
}

.routeIcon {
  stroke: $color-grey-dark;
  transform: scale(.8);
  cursor: pointer;

  &.selected {
    stroke: $color-near-white;
  }
}

.open {
  transform: rotate(0deg);
}

</style>
