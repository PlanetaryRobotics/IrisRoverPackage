<template>
  <div id = "routeManager" class="scrollable">
    <!-- TAB -->
    <div class="mapTab">
      <!-- HEADER --> 
      <div class="mapTab__header" @click="toggleRoutes">
        <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="mapTab__icon" :class="{ open : show.routes }">
        <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
        </svg>
        <h2 class="text__main--bold mapTab__title">Routes</h2>
      </div>

      <!-- ROUTE LIST --> 
      <div class="mapTab__content" v-show = "show.routes">
        <div class="routeList" v-if="!routeListIsEmpty">
          <!-- ROUTE -->
          <div class="routeList__item" v-for="(route, index) in routeList.routes" :key="index">
            <RouteEntry :route="route"/>
          </div>
        </div>
        <div v-else class="emptyRouteList">
          <div class="text">Start a route by adding waypoints or circumnavigations.</div>
          <div class="emptyRouteList__button">
            Start
          </div>
        </div>
      </div>
    </div>
    <!-- END TAB -->

    <AtomicButton v-bind="buttons.addRoute" 
                         @click.native="addRoute"
                         v-show="!routeListIsEmpty"/>
  </div>
</template>

<script>

import { mapMutations, mapGetters } from 'vuex';
import RouteEntry from "@/components/Map/MapComponents/RouteManager/RouteEntry.vue";
import AtomicButton from '@/components/atomic/AtomicButton.vue';

export default {
  name: "RouteManager",
  components: {
    RouteEntry,
    AtomicButton
  },
  data() {
    return {
      show: {
        routes: false,
      },
      route: {
        visible: true
      },
      buttons: {
        addRoute: {
          id:'addRouteButton',
          flavor:'primary',
          text:'Add Route',
          value: 'addRoute',
          enabled: true,
          storeId: 'MAP'
        }
      }
    }
  },
  computed: {
    ...mapGetters(['routeList', 'routeListIsEmpty']),
  },
  mounted() {
    // Add route button positioning
    this.setAddButtonPosition();
    let that = this;
    window.onresize = function() {
      that.setAddButtonPosition();
    };
  },
  methods: {
    ...mapMutations({
      triggerRouteListUpdate: 'triggerRouteListUpdate'
    }),
    toggleRoutes() {
      this.show.routes = !this.show.routes;
    },
    toggleVisibility(route) {
      route.isVisible = !route.isVisible;
      this.triggerRouteListUpdate();
    },
    setAddButtonPosition() {
      let button = document.getElementById("addRouteButton");
      let parent = document.getElementById("routeManager");
      button.style.left = parent.clientWidth/2 - button.clientWidth/2 + "px";
    },
    addRoute() {

    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_mapTab.scss';

#routeManager {
  background-color: $color-near-black;
}

.routeList {
  margin-left: 1rem;

  &__item {
    margin: 1rem 2rem 1rem 2rem;

    &--visibility {
      display: flex;
      align-items: center;
    }
  }
}

.emptyRouteList {
  padding: 2rem;
  color: $color-grey;
  height: 45vh;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  text-align: center;

  &__button {
    margin-top: 1rem;
    padding: 0.5rem 3rem 0.5rem 3rem;
    color: white;
    background-color: $color-primary;
    border-radius: 20px;

    &:hover {
      cursor: pointer;
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

#addRouteButton{
  position: absolute;
  z-index: 10;
  bottom: 43vh;
  margin: 0 auto;
}

</style>
