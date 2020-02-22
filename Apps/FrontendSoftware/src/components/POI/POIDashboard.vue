<template>
  <div class="POIDashboardContent noSelect">
    
    <div class="POIDashboardContent__leftColumn">

      <div class="POIDashboardContent__header">
        <div class="text__large header" 
              v-bind:class="{'current': show.POIManagement}"
              @click="toggleManager()">
          POI Management
        </div>
        <div class="text__large header"
            v-bind:class="{'current': !show.POIManagement}"
            @click="toggleManager()">
          Tag Management
        </div>
      </div>
      

      <div class="POIDashboardContent__body scrollable">
        <POIManagement v-if="show.POIManagement"/>
        <TagManagement v-else />
      </div>
    </div>
    

    <!-- RIGHT COLUMN -->
    <POIList class="POIDashboardContent__rightColumn"/>

  </div>
</template>

<script>

import POIList from "@/components/POI/POIList/POIList.vue";
import POIManagement from "@/components/POI/POIManagement/POIManagement.vue";
import TagManagement from "@/components/POI/TagManagement/TagManagement.vue";
// import { mapGetters } from 'vuex';

export default {
  name: "POIDashboard",
  components: {
    POIList,
    POIManagement,
    TagManagement
  },
  data() {
    return {
      show: {
        POIManagement: true
      }
    }
  },
  props: {
  },
  computed: {
    // ...mapGetters(['showPOIManagement', 'showTagManagement'])
  },
  methods: {
    toggleManager() {
      this.show.POIManagement = !this.show.POIManagement;
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';
@import '@/styles/_noSelect.scss';

.POIDashboardContent {
  border-radius: 4px;
  width: 100%;
  display: grid;
  grid-template-columns: 3fr 1fr;

  &__leftColumn {
    height: 95vh;
    padding-left: 34px 8px 34px 0px;
    background-color: $color-background;
    display: flex;
    flex-direction: column;
  }

  &__body {
    padding-left: 34px;
    padding-right: 34px;
  }

  &__header {
    padding-bottom: 16px;
  }

  &__rightColumn {
    height: 95vh;
    background-color: $color-near-black;
  }
}

.icon {
  &:hover * {
    cursor: pointer;
    stroke: $color-primary;
  }
}

.header {
  display: inline-block;
  padding-left: 34px;
  padding-right: 34px;
  line-height: 4rem;
  border-bottom: 4px solid #9F9F9F;
  color: #9F9F9F;

  &:hover {
    cursor: pointer;
  }
}

.current {
  color: white;
  border-bottom: 4px solid white;
}
</style>
