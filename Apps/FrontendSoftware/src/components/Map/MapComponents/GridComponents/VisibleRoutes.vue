<template>
  <g id="visibleRoutes">
    <Route
      v-for="(route) in routeList.routes"
      :key="route.uuid"
      :route="route"
      :origin="origin"
      :grid-square="gridSquare"
    />
  </g>
</template>

<script>

import { mapGetters } from 'vuex';
import Route from '@/components/Map/MapComponents/GridComponents/Route.vue';

export default {
    name: 'VisibleRoutes',
    components: {
        Route
    },
    props: ['origin', 'gridSquare'],
    computed: {
        ...mapGetters({
            routeList: 'routeList',
        }),
    },
    watch: {
    },
    methods: {
    }
};

</script>

<style lang="scss" scoped>
#visibleRoutes {
  &:hover {
    cursor:pointer;
  }
}
</style>
