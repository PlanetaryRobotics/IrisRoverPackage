<template>
    <div class="map-container noSelect">

      <div class="map-body">
        <!-- LEFT NAVIGATION --> 
        <MapNavigationLeft />

        <!-- GRID --> 
        <Grid />

        <!-- RIGHT PANEL --> 
        <div class="right-panel">
          <div class="right-panel__container" v-show="show.routesAndFeatures">
            <Routes />
            <POIList />
          </div>
          <div v-show="show.routeManager">
            <CreateRoute />
            <AddToRoute />
          </div>
        </div>
      </div>

      <!-- RIGHT NAVIGATION --> 
      <div id="mapNavigationRight">
        <div class="functional menu">
            <RadioSVGButton :id="'RouteAndFeaturesRadioSVG'"
                  :initiallyChecked="true"
                  :value="'true'"
                  :svgName="'icon_visible'" 
                  :storeId="'MAP'"
                  class="icon"
                  v-on:click.native="switchRightPanelView('routesAndFeatures')"
                  />
            
            <RadioSVGButton :id="'RouteManagerRadioSVG'"
                  :initiallyChecked="false"
                  :value="'false'"
                  :svgName="'icon_route_manager'" 
                  :storeId="'MAP'"
                  class="icon"
                  v-on:click.native="switchRightPanelView('routeManager')"
                  />
        </div>
      </div>
    <!-- END MAP CONTAINER --> 
    </div>
</template>

<script>

import { mapGetters } from 'vuex';
import MapNavigationLeft from '@/components/Map/MapComponents/MapNavigationLeft.vue';
import Grid from '@/components/Map/MapComponents/Grid.vue';
import POIList from '@/components/POI/POIList/POIList.vue';
import Routes from '@/components/Map/MapComponents/RouteManager/Routes.vue';
import CreateRoute from '@/components/Map/MapComponents/FormComponents/CreateRoute.vue';
import AddToRoute from '@/components/Map/MapComponents/FormComponents/AddToRoute.vue';
import RadioSVGButton from '@/components/atomic/RadioSVGButton.vue';

export default {
  name: "Map",
  components: {
    MapNavigationLeft,
    Grid,
    Routes,
    POIList,
    // POIDashboard,
    CreateRoute,
    AddToRoute,
    RadioSVGButton
  },
  data() {
    return {
      show: {
        routesAndFeatures: true,
        routeManager: false,
      }
    }
  },
  computed: {
    ...mapGetters(['POIDashboardOpen'])
  },
  methods: {
    switchRightPanelView(state) {
      if (state === "routeManager") {
        this.show.routeManager = true;
        this.show.routesAndFeatures = false;
      } else {
        this.show.routeManager = false;
        this.show.routesAndFeatures = true;
      }
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_noSelect.scss';
@import '@/styles/_functional.scss';

.map-container{
  position: relative;
  display: grid;
  grid-template-columns: 1fr 4rem;
  grid-template-rows: 1fr;
  grid-gap: 2px;
  overflow: hidden;
}

.map-body {
  display: grid;
  grid-template-columns: 4rem 2fr 1fr;
}

.right-panel {
  z-index: 10;
  background-color: $color-near-black;

  &__container {
    height: 95vh;
    position: relative;
    display: grid;
    grid-template-rows: 1fr 2fr;
  }
}

.functional {
  display: grid;
  user-select: none;
  height: 100%;
  grid-template-rows: min-content 1fr min-content;
  background-color: $color-background;

  &.menu {
    padding-top: 1rem;

    &__option {
      padding: 0.8rem 0.5rem;
      cursor: pointer;
    }
  }
}

.icon {
  padding: 1rem 1rem;
  cursor: pointer;
}

</style>
