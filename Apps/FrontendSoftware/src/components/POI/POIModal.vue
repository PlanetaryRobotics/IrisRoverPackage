<template>
  <div class="POIModalContent">
    <div class="POIModalContent__manager">
      <!-- HEADERS -->
      <div class="POIModalContent__header">
        <div class="text__large header" 
              v-bind:class="{'current': current.POI}"
              @click="toggleManager()">
          POI Management
        </div>
        <div class="text__large header"
            v-bind:class="{'current': current.Tag}"
            @click="toggleManager()">
          Tag Management
        </div>
      </div>
      
      <!-- MAIN CONTENT -->
      <div class="POIModalContent__body scrollable">
        <POIManagement v-if="showPOIManagement"/>
      </div>
    </div>

    <!-- RIGHT COLUMN -->
    <POIList class="POIModalContent__rightColumn"/>

  </div>
</template>

<script>

import POIList from "@/components/POI/POIList.vue";
import POIManagement from "@/components/POI/POIManagement/POIManagement.vue";
import { mapGetters } from 'vuex';

export default {
  name: "POIModal",
  components: {
    POIList,
    POIManagement
  },
  data() {
    return {
      current: {
        POI: true,
        Tag: false
      }
    }
  },
  props: {
  },
  computed: {
    ...mapGetters(['showPOIManagement', 'showTagManagement'])
  },
  methods: {
    toggleModal() {
      this.$store.commit("toggleModal");
    },
    toggleManager() {
      this.current.POI = !this.current.POI;
      this.current.Tag = !this.current.Tag;
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.POIModalContent {
  border-radius: 4px;
  height: 95vh; // TODO: do not use absolute vals, fix the stretching issue on parent components
  width: 100%;
  display: grid;
  grid-template-columns: 3fr 1fr;
  overflow: hidden;

  &__body {
    height: 95vh; // TODO: do not use absolute vals, fix the stretching issue on parent components
    padding-left: 34px;
    padding-right: 34px;
  }

  &__header {
    padding-bottom: 16px;
  }

  &__manager {
    padding-left: 34px 8px 34px 0px;
    background-color: $color-background;
  }

  &__rightColumn {
    height: 95vh; // TODO: do not use absolute vals, fix the stretching issue on parent components
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
