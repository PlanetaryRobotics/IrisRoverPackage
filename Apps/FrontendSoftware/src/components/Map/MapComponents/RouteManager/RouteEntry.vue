<template>
  <div class="routeEntry">

    <!-- ROUTETAB -->
    <div class="routeTab">
      <!-- DOWN ARROW -->
      <div class="routeTab__header" @click="toggleSegmentList">
        <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="routeTab__icon" :class="{ open : show.segmentList }">
          <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
        </svg>

        <!-- ROUTE NAME --> 
        <p class="route__item--name text__main"> {{route.routeName}} </p>
      </div>

      <!-- PLUS ICON -->   
      <div class="route__item--plus" @click="openAddModal()">
        <svg width="17" height="17" viewBox="0 0 17 17" fill="none" xmlns="http://www.w3.org/2000/svg" class="routeIcon selected">
          <line x1="8.5" y1="4.5" x2="8.5" y2="12.5" stroke="#333333" stroke-linecap="round" stroke-linejoin="round"/>
          <line x1="4.5" y1="8.5" x2="12.5" y2="8.5" stroke="#333333" stroke-linecap="round" stroke-linejoin="round"/>
          <circle cx="8.5" cy="8.5" r="8" stroke="#333333"/>
        </svg>
      </div>

      <!-- VISIBILITY ICON -->
      <div class="route__item--visibility" @click="toggleVisibility(route)" >
        <svg width="19" height="14" viewBox="0 0 19 14" fill="none" xmlns="http://www.w3.org/2000/svg" class="routeIcon" :class="{selected: route.isVisible}" >
          <circle cx="9.50019" cy="6.88459" r="2.76923"/>
          <path d="M9.5 1C4.92308 1 1 4.92308 1 6.88462C1 8.84615 4.92308 12.7692 9.5 12.7692C14.0769 12.7692 18 8.84615 18 6.88462C18 4.92308 14.0769 1 9.5 1Z"/>
        </svg>
      </div>
    </div> <!-- END ROUTETAB -->
  
    <!-- ROUTEBODY -->
    <div class="routeBody">

      <div class="line" />

      <!-- SEGMENT LIST -->
      <div class="segmentList" v-show = "show.segmentList">
        <div class="segmentList__segment" v-for="(segment, index) in route.segmentList" :key="index">
          <SegmentInfo :segment = "segment" :index="index"/>
        </div>
      </div> 

    </div> <!-- END ROUTEBODY -->
  </div>  <!-- END ROUTE ENTRY -->
  
</template>

<script>
import GridEventBus from '@/components/Map/GridEventBus.js';
import SegmentInfo from "@/components/Map/MapComponents/RouteManager/SegmentInfo.vue";
import { mapMutations, mapGetters } from 'vuex';

export default {
  name: "RouteEntry",
  components: {
    SegmentInfo
  },
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
    },
    openAddModal() {
      GridEventBus.$emit("OPEN_ADD_MODAL");
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_mapTab.scss';

.routeTab {
  display: flex;
  flex-direction: row;
  justify-content: flex-start;

  &__header {
    display: flex;
    justify-content: flex-end;
    align-items: center;
    cursor: pointer;
    user-select: none;
    flex-grow: 1;
  }

  &__icon {
    stroke: $color-grey;
    width: 14px;
    height: 14px;
    transform: rotate(-90deg);
    transition: .1s ease-in-out;

    &:hover {
      stroke: $color-primary;
    }
  }

  &__name {
    padding-left: 0.5rem;
  }
}

.routeBody {
  display: flex;
  flex-direction: row;
}

.line {
  border-left: 1px solid $color-grey-dark;
  margin-top: 0.5rem;
  margin-left: 0.5rem;
}

.segmentList {
  margin-top: 1.5rem;
  margin-left: 2rem;
  margin-bottom: 1.5rem;
  flex-grow: 1;
  &__segment {
    width: 100%;
  }
}

.route {
  &__item {
    display: flex;
    align-items: center;
    justify-content: space-between;

    &--name {
      padding-left: 1rem;
      flex-grow: 1;
    }

    &--visibility {
      display: flex;
      align-items: center;
    }

    &--plus {
      display: flex;
      align-items: center;
      margin-right: 0.5rem;
    }
  }
}

.routeIcon {
  stroke: $color-grey-dark;
  transform: scale(.8);
  cursor: pointer;

  &.selected {
    stroke: $color-near-white;
    * {
      stroke: $color-near-white;
    } 
  }

  &:hover {
    > * {
      stroke: $color-primary;
    }
  }
}

.open {
  transform: rotate(0deg);
}

</style>
