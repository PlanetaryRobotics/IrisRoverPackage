<template>
  <div class="POIManagement scrollable">
    <div
      v-for="(POICard, index) in POIList"
      :key="index"
      class="POIManagementHeader"
    >
      <POIManagementCard
        :p-o-i-data="POICard.getData()"
        :p-o-i-card="POICard"
      />
    </div>
  </div> 
</template>

<script>

import POIManagementCard from '@/components/POI/POIManagement/POIManagementCard.vue';

export default {
    name: 'POIManagement',
    components: {
        POIManagementCard
    },
    computed: {
        POIList() {
            return this.$store.getters.POIList;
        }
    },
};

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.POIManagementHeader {
  display: flex;
  flex-direction: row;
  align-items: center;
}

</style>
