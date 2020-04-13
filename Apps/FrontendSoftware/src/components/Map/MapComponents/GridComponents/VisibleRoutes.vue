<template>
    <g id = "visibleRoutes" :key = "updateRouteListKey">
      <Route v-for="(route, idx) in routeList.routes"
             :route="route"
             :key="idx"
             :origin="origin"
             :rover="rover"
             :gridSquare="gridSquare"
             />
    </g>
</template>

<script>

import { mapGetters } from 'vuex';
import Route from '@/components/Map/MapComponents/GridComponents/Route.vue';

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
    routeListUpdate() {
      this.updateRouteListKey++;
    }
  },
  methods: {
  }
}

</script>

<style lang="scss" scoped>
#visibleRoutes {
  &:hover {
    cursor:pointer;
  }
}
</style>
