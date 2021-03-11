<template>
  <div class="noSelect">
    <SegmentModal
      v-if="show.segmentModal"
      :key="modalRerender"
      :route="this.modalRoute"
      :segment="this.modalSegment"
      :segment-index="this.modalSegmentIndex"
      :action="this.modalAction"
    />
    <div class="map-container">
      <div class="map-body">
        <!-- LEFT NAVIGATION --> 
        <MapNavigationLeft />

        <!-- GRID --> 
        <Grid />

        <!-- RIGHT PANEL --> 
        <div class="right-panel">
          <div class="right-panel__container">
            <RouteManager />
            <POIList />
          </div>
        </div>
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
import RouteManager from '@/components/Map/MapComponents/RouteManager/RouteManager.vue';
import SegmentModal from '@/components/Map/MapComponents/RouteManager/SegmentModal.vue';
import GridEventBus from '@/components/Map/GridEventBus.js';

export default {
    name: 'Map',
    components: {
        MapNavigationLeft,
        Grid,
        RouteManager,
        POIList,
        SegmentModal
    },
    data() {
        return {
            show: {
                routesAndFeatures: true,
                routeManager: false,
                segmentModal: false
            },
            modalRoute: null,
            modalSegment: null,
            modalSegmentIndex: null,
            modalAction: null,
            modalRerender: 0,
        };
    },
    computed: {
        ...mapGetters(['POIDashboardOpen'])
    },
    mounted() {
        GridEventBus.$on('CLOSE_SEGMENT_MODAL', () => {
            this.show.segmentModal = false;
            this.modalAction = null;
            this.modalRoute = null;
        });

        GridEventBus.$on('OPEN_SEGMENT_MODAL', (data) => {
            // Force modal to re-render if it is already open
            // (updated data may change the type of form that is rendered, so 
            // existing one has to be forcibly overriden)
            if (this.show.segmentModal) {
                this.modalRerender++;
            }

            let {route, segment, segmentIndex, action} = data;

            this.modalRoute = route;
            this.modalSegment = segment;
            this.modalSegmentIndex = segmentIndex;
            this.show.segmentModal = true;
            this.modalAction = action;
        });
    },
    beforeDestroy() {
        GridEventBus.$off('CLOSE_SEGMENT_MODAL');
        GridEventBus.$off('OPEN_SEGMENT_MODAL');
    },
    methods: {
        switchRightPanelView(state) {
            if (state === 'routeManager') {
                this.show.routeManager = true;
                this.show.routesAndFeatures = false;
            } else {
                this.show.routeManager = false;
                this.show.routesAndFeatures = true;
            }
        }
    }
};

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_noSelect.scss';
@import '@/styles/_functional.scss';

.map-container{
  position: relative;
  display: grid;
  grid-template-columns: 1fr;
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
    grid-template-rows: 2fr 1.5fr;
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
