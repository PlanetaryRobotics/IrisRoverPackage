<template>
  <div class="POIDashboardContent noSelect">
    
    <!-- SIDE SCROLLER FOR TAG MANAGEMENT -->
    <div class="POIDashboardContent__alphabetScroller">
      <AlphabetScroller v-show="!show.POIManagement"/>
    </div>

    <!-- DASHBOARD BODY -->
    <div class="POIDashboardContent__leftColumn">

      <!-- HEADERS --> 
      <div class="POIDashboardContent__header">
        <div class="text__large header POIHeader" 
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
      

      <!-- BODY -->
      <div class="POIDashboardContent__body scrollable">
        <POIManagement v-show="show.POIManagement"/>
        <TagManagement v-show="!show.POIManagement" />
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
import AlphabetScroller from "@/components/POI/TagManagement/AlphabetScroller.vue";

export default {
  name: "POIDashboard",
  components: {
    POIList,
    POIManagement,
    TagManagement,
    AlphabetScroller
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
  grid-template-columns: 38px 3fr 1fr;

  &__leftColumn {
    height: 95vh;
    background-color: $color-background;
    display: flex;
    flex-direction: column;
    padding-left: 8px;
  }

  &__body {
    padding-right: 30px;
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
  display: inline;
  padding-right: 34px;
  padding-bottom: 4px;
  line-height: 6rem;
  border-bottom: 4px solid #9F9F9F;
  color: #9F9F9F;
  text-align: center;

  &:hover {
    cursor: pointer;
  }
}

.current {
  color: white;
  border-bottom: 4px solid white;
}
</style>
