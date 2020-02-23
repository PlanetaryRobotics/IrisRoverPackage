<template>
  <div class="POIList scrollable">
    <div class="mapTab">
      <!-- HEADER --> 
      <div class="mapTab__header" @click="togglePOIList">
        <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="mapTab__icon" :class="{ open : show.POIList }">
        <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
        </svg>
        <h2 class="text__main--bold mapTab__title">Point of Interests</h2>
      </div>

      <!-- FILTER -->
      <div class="POIFilter" v-show = "show.POIList">
        <div class="POIFilter__FiltersRow">
          <div class="FilterTag" :class="{'selected': selectedFilter === 'ATTRACTION'}" @click="filterBy('ATTRACTION')" >
            Attraction
          </div>
          <div class="FilterTag" :class="{'selected': selectedFilter === 'OBSTACLE'}" @click="filterBy('OBSTACLE')">
            Obstacle
          </div>
          <div class="FilterTag" :class="{'selected': selectedFilter === 'SHADOW'}" @click="filterBy('SHADOW')">
            Shadow
          </div>
        </div>
      </div>

      <!-- SEARCH -->
      <div class="POISearch">
        <input type="text" 
                placeholder="Search"
                v-model="searchQuery" 
                />
      </div>

      <!-- ORDER BY -->
      <div class="POIOrderBy">
          <select name="POIOrderBy__dropdown" class="POIOrderBy__dropdown" v-model="orderBy"
          :style="{ background: `url(${arrowSVG}) no-repeat 95% #585858` }">
              <option value="null" selected disabled>Filter</option>
              <option value="IMPORTANCE">Importance</option>
              <option value="TIME ADDED">Time Added</option>
          </select>
      </div>

      <!-- LIST --> 
      <div class="POIList__list scrollable" v-show = "show.POIList">
        <div class="POIList__item" v-for="(POICard, index) in POIList" :key="index">
          <POICard :POIData = "POICard.getData()" :searchQuery = "searchQuery"/>
        </div>
      </div>
    </div>
  </div>
</template>

<script>

import POICard from "@/components/POI/POIList/POICard.vue";
import POIListDataClass from "@/data_classes/POIList.js";
import arrowSVG from "@/assets/icons/icon_arrow_white.svg";

// import { mapGetters } from 'vuex';

export default {
  name: "POIList",
  components: {
    POICard
  },
  computed: {
    POIList() {
      let POIList = this.$store.getters.POIList;
    
      let list = POIListDataClass.filterBy(this.selectedFilter, POIList);
      list = POIListDataClass.orderBy(this.orderBy, list);

      return list;
    }
  },
  data() {
    return {
      show: {
        POIList: true,
      },
      selectedFilter: null,
      orderBy: null,
      searchQuery: null,
      arrowSVG: arrowSVG
    }
  },
  methods: {
    togglePOIList() {
      this.show.POIList = !this.show.POIList;
    },
    filterBy(filter) {
      if (this.selectedFilter === filter) {
        this.selectedFilter = null;
      } else {
        this.selectedFilter = filter;
      }
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
  }
}

.POIFilter {
  display: flex;
  flex-direction: column;

  &__FiltersRow {
    margin-top: 12px;
    display: flex;
    flex-direction: row;
    flex-wrap: wrap;
    justify-content: space-between;

    >select:focus {
      outline: none;
    }
  }
}

.POISearch {
  margin: 2rem 0 2rem 0;

  > input {
    color: $color-grey;
    width: 100%;
    padding: 8px 8px 8px 12px;
    border-radius: 4px;
    background-color: $color-grey-dark;
    border: none;
    font-size: 1.4rem;

    &::placeholder { /* Chrome, Firefox, Opera, Safari 10.1+ */
      color: $color-grey;
      opacity: 1; /* Firefox */
    }

    &:-ms-input-placeholder { /* Internet Explorer 10-11 */
      color: $color-grey;
    }

    &::-ms-input-placeholder { /* Microsoft Edge */
      color: $color-grey;
    }

    &:focus {
      outline: none !important;
      border:1px solid $color-primary;
      box-shadow: 0 0 5px #719ECE;
    }
  }
}

.POIOrderBy {
  width: 100%;
  margin-top: 2rem;

  &__dropdown {
    color: $color-grey;
    font-size: 1.4rem;
    border-radius: 4px;
    border: none;
    padding: 8px 8px 8px 12px;
    width: 100%;
    -webkit-appearance: none; 
    background-color: white;

    &:focus {
      outline: none !important;
      border:1px solid $color-primary;
      box-shadow: 0 0 5px #719ECE;
    }
  }
}

.FilterTag {
  padding: 3px 16px;
  border-radius: 20px;
  border: 1px solid $color-grey-dark;
  color: white;
}

.selected, .FilterTag:hover {
  border: 1px solid $color-primary;
  background-color: $color-primary;
  color: white;
  cursor: pointer;
}
</style>
