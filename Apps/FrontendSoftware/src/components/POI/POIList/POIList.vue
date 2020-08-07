<template>
  <div id="POIList" ref="POIList" class="POIList scrollable">
    <!-- EDIT WINDOW -->
    <div v-if = "show.editWindow">
      <POIEdit :POICard = "POICardToEdit.cardObject" 
               :POIListEl = "POIListEl"/>
    </div>
    
    <!-- REGULAR VIEW -->
    <div v-else>
      <div class="mapTab dark" >
        <!-- HEADER --> 
        <div class="mapTab__header" @click="togglePOIList">
          <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="mapTab__icon" :class="{ open : show.POIList }">
          <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
          </svg>
          <h2 class="text__main--bold mapTab__title">Point of Interests</h2>
        </div>

        <div class="mapTab__body" v-show = "show.POIList">
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
            <!-- SEARCH BAR -->
            <input type="text" 
                    placeholder="Search"
                    v-model="searchQuery" 
                    />
            <!-- ORDER BY BUTTON -->
            <svg @click="handleOrderClick" class="POIOrderBy" width="20" height="20" viewBox="0 0 14 14" fill="none" xmlns="http://www.w3.org/2000/svg">
              <path d="M4.74998 2.25C4.74998 2.11193 4.63805 2 4.49998 2C4.3619 2 4.24998 2.11193 4.24998 2.25V11.0364L2.42678 9.21323C2.32915 9.11559 2.17086 9.11559 2.07322 9.21322C1.97559 9.31085 1.97559 9.46915 2.07322 9.56678L4.49997 11.9936L6.92675 9.56678C7.02438 9.46915 7.02438 9.31086 6.92675 9.21323C6.82912 9.11559 6.67083 9.11559 6.5732 9.21323L4.74998 11.0364V2.25Z" fill="#FCFCFC"/>
              <path d="M9.25002 11.75C9.25002 11.8881 9.36195 12 9.50002 12C9.6381 12 9.75002 11.8881 9.75002 11.75L9.75003 2.96356L11.5732 4.78677C11.6709 4.88441 11.8291 4.88441 11.9268 4.78678C12.0244 4.68915 12.0244 4.53085 11.9268 4.43322L9.50003 2.00644L7.07325 4.43322C6.97562 4.53085 6.97562 4.68914 7.07325 4.78677C7.17088 4.88441 7.32917 4.88441 7.4268 4.78677L9.25003 2.96355L9.25002 11.75Z" fill="#FCFCFC"/>
            </svg>
          </div>

          <!-- IMAGE INFO -->
          <div class="ImageInfo" v-if="selectedImage">
            <div class="ImageInfo__header text__main--bold">Image</div>
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
              800x600 
            </div>
          </div>

          <!-- TAG INFO -->
          <div class="ImageInfo" v-if="selectedTag">
            <div class="ImageInfo__row">
              <div class="pill__tag">
                {{selectedTag.getName()}}
              </div>
            </div>
            <div class="ImageInfo__row">
              {{selectedTag.data.description}}
            </div>
            <div class="ImageInfo__history">
              <div class="updateHistorySmall--line text__small">
                <div>Created:</div> 
                <div>{{selectedTag.data.modificationHistory[0].user}}</div>
                <div>{{selectedTag.data.modificationHistory[0].time}}</div>
              </div>
              <div v-if = "selectedTag.data.modificationHistory.length > 1" class="updateHistorySmall--line text__small">
                <div>Modified:</div>
                <div>{{selectedTag.data.modificationHistory[selectedTag.data.modificationHistory.length-1].user}}</div> 
                <div>{{selectedTag.data.modificationHistory[selectedTag.data.modificationHistory.length-1].time}}</div>
              </div>
            </div>
          </div>

          <!-- LIST --> 
          <div class="POIList__list scrollable">
            <div class="POIList__item" v-for="(POICard) in POIList" :key="POICard.uuid">
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
    ...mapGetters(['POIImageSelected', 'TagSelected']),
    POIList() {
      let POIList = this.$store.getters.POIList;
      let list = POIListDataClass.filterBy(this.selectedFilter, POIList);

      let order;
      if (this.orderImportanceByMost) {
        order = "IMPORTANCE-MOST-TO-LEAST"
      } else {
        order = "IMPORTANCE-LEAST-TO-MOST"
      }

      list = POIListDataClass.orderBy(order, list);

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
    TagSelected: {
      deep: true,
      handler(newObj) {
        this.selectedTag = newObj.tag;
      }
    }
  },
  data() {
    return {
      show: {
        POIList: true,
        editWindow: false,
      },
      selectedFilter: null,
      orderImportanceByMost: true,
      searchQuery: null,
      arrowSVG: arrowSVG,
      POICardToEdit: {
        cardObject: null,
        JSON: null,
      },
      POIListEl: null,
      selectedImage: null,
      selectedTag: null
    }
  },
  mounted() {
    this.POIListEl = this.$refs.POIList;
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
  beforeDestroy() {
    POIEventBus.$off('OPEN_EDIT_POI_WINDOW');
    POIEventBus.$off('CLOSE_EDIT_POI_WINDOW');
    POIEventBus.$off('DELETE_POI_WINDOW');
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
    },
    handleOrderClick() {
      this.orderImportanceByMost = !this.orderImportanceByMost;
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_mapTab.scss';
@import '@/styles/_pill.scss';

.POIList {
  &__item {
    border-radius: 4px;
    background-color: $color-background;
    margin-top: 2rem;
  }
}

.dark {
  background-color: $color-near-black;
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
    border-bottom: 1px solid $color-grey-dark;

    >select:focus {
      outline: none;
    }
  }
}

.POISearch {
  display: flex;
  flex-direction: row;
  align-items: center;
  margin: 2rem 0 0 0;

  > input {
    margin-right: 1rem;
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

  > .POIOrderBy {
    &:hover {
      cursor: pointer;

      > * {
        fill: $color-primary;
      }
    }
  }
}

.FilterTag {
  width: 33.33%;
  display: flex;
  justify-content: center;
  align-items: center;
  border-radius: 10px 10px 0 0;
  padding: 4px 0;
  color: white;
  font-weight: bold;
}

.selected, .FilterTag:hover {
  border: 1px solid $color-grey-dark;
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
    margin: 0rem 0 1rem 0;
  }

  &__property {
    margin-right: 1rem;
  }

  &__history {
    display: flex;
    flex-direction: column;
  }
}

.updateHistorySmall {
  padding-top: 1rem;
  padding-bottom: 2rem;

  > div {
    padding-right: 2px;
  }

  > div:nth-child(1) {
    padding-bottom: 1rem;
  }

  &--line {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
  }
}

</style>
