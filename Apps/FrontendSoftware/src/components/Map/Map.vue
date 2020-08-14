<template>
  <div id="map">
    <div class="map-container">
      <MapNavigationLeft />
      <Grid />
      <div class="right-panel">
        <Routes />
        <div>
          FEATURES HERE
        </div>
      </div>
      <MapNavigationRight />
    </div>
  </div>
</template>

<script>

import MapNavigationLeft from '@/components/Map/MapComponents/MapNavigationLeft.vue';
import MapNavigationRight from '@/components/Map/MapComponents/MapNavigationRight.vue';
import Grid from '@/components/Map/MapComponents/Grid.vue';
import Routes from '@/components/Map/MapComponents/Routes.vue';

export default {
  name: "Map",
  components: {
    MapNavigationLeft,
    MapNavigationRight,
    Grid,
    Routes
  },
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';

.map-container{
  position: relative;
  height: 100%;
  display: grid;
  grid-template-columns: 4rem 2fr 1fr 4rem;
  grid-gap: 2px;
}

.right-panel {
  background-color: grey;
  height: 100%;
  display: grid;
  grid-template-rows: 2fr 3fr;
  grid-gap: 2px;
}

</style>
