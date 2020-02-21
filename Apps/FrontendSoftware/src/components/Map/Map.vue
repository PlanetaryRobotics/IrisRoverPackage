<template>
    <div class="map-container">

      <POIModal v-if="POIModalOpen" />

      <div class="map-body" v-else>
        <!-- LEFT NAVIGATION --> 
        <MapNavigationLeft />

        <!-- GRID --> 
        <Grid />

        <!-- RIGHT PANEL --> 
        <div class="right-panel">
          <div class="right-panel__container" v-show="show.routesAndFeatures">
            <Routes class="right-panel--top"/>
            <POIList class="right-panel--bottom"/>
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
import POIList from '@/components/POI/POIList.vue';
import POIModal from '@/components/POI/POIModal.vue';
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
    POIModal,
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
    ...mapGetters(['POIModalOpen'])
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

.map-container{
  position: relative;
  max-height: 100%;
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
  position: relative;
  height: 100%;

  &__container {
    height: 100%;
    position: relative;
  }

  &--top {
    height: 30vh; 
    position: relative;
  }

  &--bottom {
    height: 65vh; //TODO: some bug with this, should be using %, temp fix w/ vh for now
    position: relative;
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
