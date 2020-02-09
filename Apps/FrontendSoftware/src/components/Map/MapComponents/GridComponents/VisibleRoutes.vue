<template>
    <g id = "visibleRoutes" :key = "updateRouteListKey">
      <Route v-for="(route, idx) in routeList"
             :route="route"
             :key="idx"
             :origin="origin"
             :rover="rover"
             :gridSquare="gridSquare"
             />
    </g>
</template>

<script>

import $ from 'jquery';
import * as d3 from "d3";
import { mapGetters } from 'vuex';
import Route from '@/components/Map/MapComponents/GridComponents/Route.vue';
import GridEventBus from '@/components/Map/GridEventBus.js';

export default {
  name: "VisibleRoutes",
  props: ['origin', 'rover', 'gridSquare'],
  components: {
    Route
  },
  data() {
    return {
      updateRouteListKey: 0
    }
  },
  computed: {
    ...mapGetters({
      routeList: 'routeList',
      routeListUpdate: 'routeListUpdate'
    }),
  },
  mounted() {
    this.updateRouteListKey++;
  },
  watch: {
    routeList: {
      deep: true, 
      handler(routeList){
        this.updateRouteListKey++;
      }
    },
    routeListUpdate(newVal, oldVal) {
      this.updateRouteListKey++;
    }
  },
  methods: {
  }
}

</script>

<style lang="scss" scoped>

</style>
