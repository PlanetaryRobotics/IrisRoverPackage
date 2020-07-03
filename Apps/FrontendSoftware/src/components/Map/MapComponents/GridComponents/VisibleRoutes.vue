<template>
    <g id = "visibleRoutes" >
      <Route v-for="(route) in routeList.routes"
             :route="route"
             :key="route.uuid"
             :origin="origin"
             :gridSquare="gridSquare"
             />
    </g>
</template>

<script>

import { mapGetters } from 'vuex';
import Route from '@/components/Map/MapComponents/GridComponents/Route.vue';

export default {
  name: "VisibleRoutes",
  props: ['origin', 'gridSquare'],
  components: {
    Route
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
