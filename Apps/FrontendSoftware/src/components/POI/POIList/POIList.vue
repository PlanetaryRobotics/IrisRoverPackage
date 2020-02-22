<template>
  <div class="POIList scrollable">
    <!-- HEADER --> 
    <div class="mapTab">
      <div class="mapTab__header" @click="togglePOIList">
        <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="mapTab__icon" :class="{ open : show.POIList }">
        <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
        </svg>
        <h2 class="text__main--bold mapTab__title">Point of Interests</h2>
      </div>

      <!-- LIST --> 
      <div class="POIList__list scrollable" v-show = "show.POIList">
        <div class="POIList__item" v-for="(POICard, index) in POIList" :key="index">
          <POICard :POIData = "POICard.getData()"/>
        </div>
      </div>
    </div>
  </div>
</template>

<script>

import POICard from "@/components/POI/POIList/POICard.vue";
import { mapGetters } from 'vuex';

export default {
  name: "POIList",
  components: {
    POICard
  },
  computed: {
    ...mapGetters({
      POIList: 'POIList',
    }),
  },
  data() {
    return {
      show: {
        POIList: false,
      }
    }
  },
  methods: {
    togglePOIList() {
      this.show.POIList = !this.show.POIList;
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_mapTab.scss';

.POIList {
  &__item {
    border-radius: 4px;
    background-color: $color-background;
    margin-top: 2rem;
    margin-bottom: 2rem;
  }
}
</style>
