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
      </div>
    </div>
    <!-- END TAB -->
    <div class="buttonContainer">
          <AtomicButton v-bind="buttons.addRoute" 
                  @click.native="addEmptyRoute"
                  v-show = "show.routes" />
    </div>
  </div>
</template>

<script>

import { mapGetters } from 'vuex';
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
  watch: {
    routeListIsEmpty() {
      this.setAddButtonPosition();
    }
  },
  methods: {
    toggleRoutes() {
      this.show.routes = !this.show.routes;
      if (this.show.routes) {
        this.setAddButtonPosition()
      }
    },
    setAddButtonPosition() {
      let button = document.getElementById("addRouteButton");
      let parent = document.getElementById("routeManager");
      button.style.left = parent.clientWidth/2 - button.clientWidth/2 + "px";
    },
    addEmptyRoute() {
      this.$store.commit("createEmptyRoute");
    },
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_mapTab.scss';

#routeManager {
  background-color: $color-near-black;
  height: 100%;
}

.routeList {
  &__item {
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

.buttonContainer {
  display: flex;
  justify-content: center;
  align-items: center;
  position: absolute;
  bottom: 43vh;
  width: 100%;
}

</style>
