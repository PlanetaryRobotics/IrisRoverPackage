<template>
    <div class="POIManagement scrollable">
        <div class="POIManagementHeader" v-for="(POICard, index) in POIList" :key="index">
          <POIManagementCard :POIData="POICard.getData()" :POICard="POICard"/>
        </div>
    </div> 
</template>

<script>

import POIManagementCard from "@/components/POI/POIManagement/POIManagementCard.vue";

export default {
  name: "POIManagement",
  components: {
    POIManagementCard
  },
  data() {
    return {
      show: {
        images: true,
      }
    }
  },
  computed: {
    POIList() {
      return this.$store.getters.POIList;
    }
  },
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.POIManagementHeader {
  display: flex;
  flex-direction: row;
  align-items: center;

  &__title {
    flex-grow: 1;
  }

  &__toggle {
    transform: rotate(-180deg);
    transition: .1s ease-in-out;
  }
}

</style>
