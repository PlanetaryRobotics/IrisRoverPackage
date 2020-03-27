<template>
  <div id="POIList" ref="POIList" class="POIList scrollable">
    <!-- EDIT WINDOW -->
    <div v-if = "show.editWindow">
      <POIEdit :POICard = "POICardToEdit.cardObject" 
               :POIListEl = "POIListEl"/>
    </div>
    
    <!-- REGULAR VIEW -->
    <div v-else>
      <div class="mapTab" >
        <!-- HEADER --> 
        <div class="mapTab__header" @click="togglePOIList">
          <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="mapTab__icon" :class="{ open : show.POIList }">
          <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
          </svg>
          <h2 class="text__main--bold mapTab__title">Point of Interests</h2>
        </div>

        <div v-show = "show.POIList">
          <!-- FILTER -->
          <div class="POIFilter">
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
                  <option value="null" selected>Filter</option>
                  <option value="IMPORTANCE">Importance</option>
                  <option value="TIME ADDED">Time Added</option>
              </select>
          </div>

          <!-- IMAGE INFO -->
          <div class="ImageInfo" v-if="selectedImage">
            <div class="ImageInfo__header text__main--bold">Images</div>
            <div class="ImageInfo__row">
              <div class="text__main--bold ImageInfo__property">Src Name: </div> 
              {{selectedImage.name()}}
            </div>
            <div class="ImageInfo__row">
              <div class="text__main--bold ImageInfo__property">Modified:</div> 
              {{selectedImage.timeForTagFormatting}}
            </div>
            <div class="ImageInfo__row">
              <div class="text__main--bold ImageInfo__property">Dimension: </div> 
              UNKNOWN?
            </div>
            <div class="ImageInfo__row">
              <div class="text__main--bold ImageInfo__property">Detail: </div> 
              UNKNOWN?
            </div>
          </div>

          <!-- LIST --> 
          <div class="POIList__list scrollable" :key="rerenderList">
            <div class="POIList__item" v-for="(POICard, index) in POIList" :key="index">
              <POICard :POIData = "POICard.getData()" 
                      :searchQuery = "searchQuery"
                      :POIListEl = "POIListEl"
                      :POICard = "POICard"
                      />
            </div>
          </div> 

        </div> <!-- END V-SHOW === show.POIList -->
      </div> <!-- END MAPTAB -->
    </div> <!-- END V-ELSE CONTAINER -->
  </div> <!-- END POILIST CONTAINER -->
</template>

<script>
import { mapGetters } from 'vuex';

import POICard from "@/components/POI/POIList/POICard.vue";
import POIEdit from "@/components/POI/POIList/POIEdit.vue";

import POIListDataClass from "@/data_classes/POIList.js";
import arrowSVG from "@/assets/icons/icon_arrow_white.svg";
import POIEventBus from "@/components/POI//POIEventBus.js";

export default {
  name: "POIList",
  components: {
    POICard,
    POIEdit
  },
  computed: {
    ...mapGetters(['POIImageSelected']),
    POIList() {
      let POIList = this.$store.getters.POIList;
    
      let list = POIListDataClass.filterBy(this.selectedFilter, POIList);
      list = POIListDataClass.orderBy(this.orderBy, list);

      return list;
    },
  },
  watch: {
    POIImageSelected: {
      deep: true, 
      handler(newObj){
       this.selectedImage = newObj.image;
      }
    },
  },
  data() {
    return {
      show: {
        POIList: true,
        editWindow: false,
      },
      selectedFilter: null,
      orderBy: null,
      searchQuery: null,
      arrowSVG: arrowSVG,
      rerenderList: 0,
      POICardToEdit: {
        cardObject: null,
        JSON: null,
      },
      POIListEl: null,
      selectedImage: null
    }
  },
  created() {
    
    POIEventBus.$on('OPEN_EDIT_POI_WINDOW', (card) => {
      this.POICardToEdit.cardObject = card;
      this.POICardToEdit.JSON = JSON.stringify(card);

      this.show.editWindow = true;
    })

    POIEventBus.$on('CLOSE_EDIT_POI_WINDOW', (card) => {
      if (JSON.stringify(card) !== this.POICardToEdit.JSON) {
        card.addToModificationHistory();
      }

      this.POICardToEdit = {
        cardObject: null,
        JSON: null,
      }
      this.show.editWindow = false;
    })

    POIEventBus.$on('DELETE_POI', (card) => {
      this.$store.commit("deletePOI", card);
      this.show.editWindow = false;
    })

  },
  mounted() {
    this.POIListEl = this.$refs.POIList;
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
      this.rerenderList++;
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

.ImageInfo {
  margin-top: 2rem;
  border-top: 1px solid $color-grey-dark;
  border-bottom: 1px solid $color-grey-dark;
  padding: 2rem 0 2rem 0;

  &__header {
    margin-bottom: 2rem;
  }

  &__row {
    display: flex;
    flex-direction: row;
    margin: 1rem 0 1rem 0;
  }

  &__property {
    margin-right: 1rem;
  }
}

</style>
