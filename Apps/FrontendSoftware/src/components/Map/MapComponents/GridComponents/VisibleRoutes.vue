<template>
    <g id = "visibleRoutes" >
      <Route v-for="(route) in routeList.routes"
             :route="route"
             :key="route.uuid"
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
    }),
  },
  watch: {
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
