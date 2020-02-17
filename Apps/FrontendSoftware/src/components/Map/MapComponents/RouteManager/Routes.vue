<template>
  <div id = "routes" class="scrollable">
    <!-- HEADER --> 
    <div class="mapTab">
      <div class="mapTab__header" @click="toggleRoutes">
        <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="mapTab__icon" :class="{ open : show.routes }">
        <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
        </svg>
        <h2 class="text__main--bold mapTab__title">Routes</h2>
      </div>

      <!-- ROUTE LIST --> 
      <div class="mapTab__content" v-show = "show.routes">
        <div class="route">

          <!-- ROUTE -->
          <div class="route__item" v-for="(route, index) in routeList" :key="index">
            <RouteEntry :route="route"/>
          </div>
        </div>
      </div>

    </div>
  </div>
</template>

<script>

import $ from 'jquery';
import { mapMutations, mapGetters } from 'vuex';
import RouteEntry from "@/components/Map/MapComponents/RouteManager/RouteEntry.vue";

export default {
  name: "Routes",
  components: {
    RouteEntry
  },
  data() {
    return {
      show: {
        routes: false,
      },
      route: {
        visible: true
      }
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
    toggleRoutes() {
      this.show.routes = !this.show.routes;
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
@import '@/styles/_mapTab.scss';

#routes {
  background-color: $color-background;
  border-bottom: 3px solid $color-near-black;
}

.route {
  padding-top: 2rem;

  &__item {
    padding-bottom: 2rem;

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

</style>
