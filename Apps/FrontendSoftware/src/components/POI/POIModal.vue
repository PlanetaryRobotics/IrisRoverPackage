<template>
    <div class="POIModalContainer" v-show="showModal">
      <div class="POIModalBackground"/>
      <div class="POIModalContent">
        <div class="POIModalContent__button" @click="toggleModal()">
          <svg class="icon" width="18" height="18" viewBox="0 0 10 11" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path class="icon__path" d="M1 1L9 10" stroke="white"/>
            <path class="icon__path" d="M8.75555 1L1.24445 10" stroke="white"/>
          </svg>
        </div>
        <div class="POIModalContent__manager">
          <div class="POIModalContent__managerHeader">
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
          <POIManagement v-if="showPOIManagement"/>
        </div>
        <POIList class="POIModalContent__rightColumn"/>
      </div>
    </div> 
</template>

<script>

import POIList from "@/components/POI/POIList.vue";
import POIManagement from "@/components/POI/POIManagement.vue";
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
    ...mapGetters(['showModal', 'showPOIManagement'])
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

.POIModalContainer {
  position: absolute;
  z-index: 99;
}

.POIModalBackground {
  background-color: black;
  opacity: 0.8;
  width: 100vw;
  height: 100vh;
}

.POIModalContent {
  background-color: $color-near-black;
  border-radius: 4px;
  position: absolute;
  height: 90%;
  width: 95%;
  z-index: 100;
  left: 50%;
  top: 30%;
  transform: translate(-50%, -30%);
  display: grid;
  grid-template-rows: 30px 1fr;
  grid-template-columns: 3fr 1fr;

  &__manager {
    padding-top: 1rem;
    grid-row: 2;
    background-color: $color-near-black;
  }

  &__managerHeader {
    display: flex;
    flex-direction: row;
    justify-content: flex-start;
  }

  &__rightColumn {
    grid-row: 2;
    background-color: $color-near-black;
  }

  &__button {
    background-color: $color-near-black;
    grid-column: 2;
    display: flex;
    justify-content: flex-end;
    align-items: center;
    margin-right: 2px;
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
  padding-left: 20px;
  padding-right: 20px;
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
