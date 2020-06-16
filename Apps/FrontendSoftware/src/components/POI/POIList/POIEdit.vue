<template>
  <div class="POIEdit" >
    <!-- SIDE MODALS -->

    <!-- IMAGE MODAL -->
    <div v-if="show.modalImages">
      <Sidemodal :key="0" 
                 :POIListEl='POIListEl' 
                 :POICard='POICard'
                 :target='this.$refs.images' 
                 :data='POICard.getData().images'
                 type='IMAGES_EDITABLE'
                 header="Images"
                 @closeModal='toggleModal'/>
    </div>
     <!-- HISTORY MODAL -->
    <div v-if="show.modalHistory">
      <Sidemodal :key="1" 
                 :POIListEl='POIListEl' 
                 :POICard='POICard'
                 :target='this.$refs.history' 
                 :data='POICard.getData().modificationHistory'
                 type="HISTORY"
                 header="Modification History"
                 @closeModal='toggleModal'/>
    </div>
    <!-- IMPORTANCE MODAL -->
    <div v-if="show.modalImportance">
      <Sidemodal :key="2" 
                 :POIListEl='POIListEl' 
                 :POICard='POICard'
                 :target='this.$refs.importance' 
                 :data='[1,2,3]'
                 type="IMPORTANCE"
                 header="Importance"
                 @closeModal='toggleModal'/>
    </div>
    <!-- END SIDE MODALS -->

    <!-- DELETE MODAL -->
    <div v-if="show.modalDelete">
      <Deletemodal :key="3"
                   :rawHTML='getDeleteModalHTML()'
                   :deleteCallback='getDeleteCallback'
                   :cancelCallback='()=>{}'
                   @closeModal='toggleModal'/>
    </div>

    <!-- END DELETE MODAL -->

    <!-- HEADER -->
    <div class="header">
      <!-- RETURN BUTTON -->
      <svg @click = "closeEdit" class="icon returnIcon" width="12" height="12" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" stroke="#ffffff">
        <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
      </svg>

      <div class = "text__main--bold">
        Edit POI
      </div>

      <!-- TRASH BUTTON -->
      <svg @click="toggleModal('modalDelete')" class="icon" width="15" height="19" viewBox="0 0 15 19" fill="none" xmlns="http://www.w3.org/2000/svg" stroke="#ffffff">
        <rect x="0.5" y="2.69006" width="13.2813" height="15.6615" rx="1.5" stroke-linecap="round"/>
        <line x1="3.77295" y1="5.07031" x2="3.77295" y2="15.9714" stroke-linecap="round"/>
        <line x1="7.04541" y1="5.07031" x2="7.04541" y2="15.9714" stroke-linecap="round"/>
        <line x1="1.68994" y1="0.5" x2="12.591" y2="0.5" stroke-linecap="round"/>
        <line x1="10.3184" y1="5.07031" x2="10.3184" y2="15.9714" stroke-linecap="round"/>
      </svg>
    </div> <!-- END HEADER -->

    <!-- TITLE -->
    <div class="title" ref="importance">

      <!-- IMPORTANCE -->
      <div class="importance" @click="toggleModal('modalImportance')">
        <div v-for="index in importanceSizeArray" :key="index">
            <svg width="2" height="16" viewBox="0 0 2 16" fill="none" xmlns="http://www.w3.org/2000/svg">
              <line x1="1" y1="-4.37114e-08" x2="1" y2="16" :style="importanceColor" stroke-width="2"/>
            </svg>
        </div>
      </div>

      <!-- CATEGORY DROPDOWN -->
      <div class="dropdown">
        <select class="dropdown__select" 
                v-model="selectedCategory"
                :style="{ background: `url(${arrowSVG}) no-repeat 90% #585858`}">
          <option v-for="(CATEGORY, index) in categories" 
                  :value="CATEGORY"  
                  :key="index">
                  {{getCategoryShortForm(CATEGORY)}}
          </option>
        </select>
      </div>

      <div class="POInumber" :style="{color: `${importanceColor}`}">
        {{"- " + POICard.getData().number}}
      </div>
    </div> <!-- END TITLE -->

    <!-- TAG TITLE -->
    <div class="tagTitle text__main--bold">
      Tags
    </div>

    <!-- TAG LIST -->
    <div class="tags">
      <div class="pill__tag--edit" v-for="(tag, index) of this.POICard.getData().tagList" :key="index" >
        {{getShortName(tag.getName())}}

        <svg @click="POICard.removeTag(tag)" class="removeButton" width="10" height="10" viewBox="0 0 10 10" fill="none" xmlns="http://www.w3.org/2000/svg">
          <path d="M8.23048 1.98065C8.31184 1.89929 8.31184 1.76738 8.23048 1.68602C8.14912 1.60466 8.01721 1.60466 7.93585 1.68602L4.95818 4.66369L1.98051 1.68603C1.89916 1.60467 1.76725 1.60467 1.68589 1.68603C1.60453 1.76738 1.60453 1.89929 1.68589 1.98065L4.66355 4.95832L1.68385 7.93802C1.60249 8.01938 1.60249 8.15129 1.68385 8.23265C1.76521 8.31401 1.89712 8.31401 1.97848 8.23265L4.95818 5.25295L7.93789 8.23266C8.01925 8.31401 8.15116 8.31401 8.23252 8.23266C8.31388 8.1513 8.31388 8.01939 8.23252 7.93803L5.25281 4.95832L8.23048 1.98065Z" fill="#FCFCFC"/>
        </svg>
      </div>
    </div>

    <!-- IMAGE AND FORM CONTAINER -->
    <div class="imageContainer">
      <img :src="POICard.getData().thumbnail.url">
      <div class="imageContainer__dimensions">

        <!-- WIDTH -->
        <div class="imageContainer__formRow">
          <div class="imageContainer__formText text__main--bold">Width: </div>
          <div class="imageContainer__input">
            <input type="text" 
                   v-model="width"/>
          </div>
          <div class="text__main">cm</div>
        </div>

        <!-- HEIGHT -->
        <div class="imageContainer__formRow">
          <div class="imageContainer__formText text__main--bold">Height: </div>
          <div class="imageContainer__input">
            <input type="text" 
                   v-model="height"/>
          </div>
          <div class="text__main">cm</div>
        </div>

        <!-- DEPTH -->
        <div class="imageContainer__formRow">
          <div class="imageContainer__formText text__main--bold">Depth: </div>
          <div class="imageContainer__input">
            <input type="text" 
                   v-model="depth"/>
          </div>
          <div class="text__main">cm</div>
        </div>
      </div>
    </div> <!-- END IMAGE FORM CONTAINER -->

    <div class="divider"/>

    <!-- DESCRIPTION TITLE -->
    <div class="descriptionTitle text__main--bold">
      Description
    </div>

    <!-- DESCRIPTION INPUT -->
    <span class="description__span">
      <div class="description__input" contenteditable="true">
        {{description}}
      </div>
    </span>

    <!-- IMAGES HEADER -->
    <div class="imagesHeader">
      <div class="imagesHeader__title text__main--bold" ref="images">
        Images
      </div>
      <div class="viewMore" @click="toggleModal('modalImages')">
        {{"View All (" + this.POICard.getData().images.length + ")  >"}}
      </div>
    </div>

    <!-- IMAGES PILLS -->
    <div class="images">
      <div class="pill__image" v-for="(image, index) of images" :key="index" >
          {{image.timeForTagFormatting}}

          <svg @click="POICard.removeImage(image)" class="removeButton" width="10" height="10" viewBox="0 0 10 10" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M8.23048 1.98065C8.31184 1.89929 8.31184 1.76738 8.23048 1.68602C8.14912 1.60466 8.01721 1.60466 7.93585 1.68602L4.95818 4.66369L1.98051 1.68603C1.89916 1.60467 1.76725 1.60467 1.68589 1.68603C1.60453 1.76738 1.60453 1.89929 1.68589 1.98065L4.66355 4.95832L1.68385 7.93802C1.60249 8.01938 1.60249 8.15129 1.68385 8.23265C1.76521 8.31401 1.89712 8.31401 1.97848 8.23265L4.95818 5.25295L7.93789 8.23266C8.01925 8.31401 8.15116 8.31401 8.23252 8.23266C8.31388 8.1513 8.31388 8.01939 8.23252 7.93803L5.25281 4.95832L8.23048 1.98065Z" fill="#FCFCFC"/>
          </svg>
      </div>
    </div>

    <!-- MODIFICATION HISTORY -->
    <div class="historyHeader">
      <div class="historyHeader__title text__main--bold" ref="history">
        Modification History
      </div>
      <div class="viewMore" @click="toggleModal('modalHistory')">
        {{"View All (" + this.POICard.getData().modificationHistory.length + ")  >"}}
      </div>
    </div>

    <div class="updateHistorySmall">
      <div class="updateHistorySmall--line text__small">
        <div>Created:</div> 
        <div>{{POICard.getData().modificationHistory[0].user}}</div>
        <div>{{POICard.getData().modificationHistory[0].time}}</div>
      </div>
      <div v-if = "POICard.getData().modificationHistory.length > 1" class="updateHistorySmall--line text__small">
        <div>Modified:</div>
        <div>{{POICard.getData().modificationHistory[POICard.getData().modificationHistory.length-1].user}}</div> 
        <div>{{POICard.getData().modificationHistory[POICard.getData().modificationHistory.length-1].time}}</div>
      </div>
    </div>

  </div>
</template>

<script>

import POICard from "@/data_classes/POICard.js";
import POIEventBus from "@/components/POI//POIEventBus.js";
import Sidemodal from "@/components/POI/Components/Sidemodal.vue";
import Deletemodal from "@/components/POI/Components/Deletemodal.vue";
import arrowSVG from "@/assets/icons/icon_arrow_white.svg";

export default {
  name: "POIEdit",
  props: {
    POICard: POICard,
    POIListEl: HTMLDivElement
  },
  components: {
    Sidemodal,
    Deletemodal
  },
  data() {
    return {
      arrowSVG: arrowSVG,
      show: {
         modalImages: false,
         modalHistory: false,
         modalImportance: false,
         modalDelete: false
      }
    }
  },
  computed: {
    description: {
      get() {
        return this.POICard.getData().description;
      },
      set(newDescription) {
        this.POICard.description = newDescription;
      }
    },
    selectedCategory: {
      get() {
        return this.POICard.getData().category;
      },
      set(newCategory) {
        this.$store.commit("updatePOICategory", {POI: this.POICard, newCategory: newCategory});
      }
    },
    width: {
      get() {
        return this.POICard.getData().width;
      },
      set(newWidth) {
        this.POICard.width = newWidth;
      }
    },
    height: {
      get() {
        return this.POICard.getData().height;
      },
      set(newHeight) {
        this.POICard.height = newHeight;
      }
    },
    depth: {
      get() {
        return this.POICard.getData().depth;
      },
      set(newDepth) {
        this.POICard.depth = newDepth;
      }
    },
    categories: function() {
      return Object.keys(POICard.CATEGORY_COLORS);
    },
    importanceColor: function() {
      let category = this.POICard.getData().category;
      let ans = POICard.CATEGORY_COLORS[category];
      return {
        stroke : ans,
      };
    },
    importanceSizeArray: function() {
      let size = this.POICard.getData().importanceLevel;
      return new Array(size);
    },
    images: function() {
      let images = this.POICard.getData().images;
      if (images.length > 4) {
        return images.slice(0, 4);
      }
      return images;
    },
  },
  methods: {
    toggleModal(key){
      this.show[key] = !this.show[key];
    },
    closeEdit() {
      POIEventBus.$emit('CLOSE_EDIT_POI_WINDOW', this.POICard);
    },
    getDeleteCallback() {
      POIEventBus.$emit('DELETE_POI', this.POICard);
    },
    getDeleteModalHTML() {
      let html = `Are you sure you want to delete the POI card `;
      let color = POICard.CATEGORY_COLORS[this.POICard.getData().category];
      html += `<span class='text__main--bold' style='color:${color}'>${this.getName()}</span>? This action cannot be undone.`;
      return html;
    },
    getName() {
      let category = this.POICard.getData().category;
      let shortname = this.getCategoryShortForm(category) + "-" + this.POICard.getData().number;
      return shortname;
    },
    getCategoryShortForm(category) {
      if (category === "ATTRACTION") {
        return "ATTR";
      }

      if (category === "SHADOW") {
        return "SHDW";
      }

      if (category === "OBSTACLE") {
        return "OBST";
      }
    },
    getShortName(name) {
      if (name.length > 7) {
        return name.substring(0, 7) + "...";
      } 
      return name;
    },
    viewMoreTagsNumber() {
      if (this.POICard.getData().tagList.length > 7) {
        return this.POICard.getData().tagList.length - 7;
      } 
      return 0;
    },

  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_pill.scss';

.header {
  display: flex;
  flex-direction: row;
  justify-content: space-between;
  align-items: center;
  margin: 14px 10px 14px 10px;
}

.icon {
  stroke: #ffffff;

  &:hover {
    stroke: $color-primary;
    cursor: pointer;
  }
}

.returnIcon {
  transform: rotate(90deg);
}

.importance {
  display: flex;
  flex-direction: row;
  width: 1rem;

  > div {
    padding-right: 2px;
  }

  &:hover {
    cursor: pointer;
  }
}

.title {
  display: flex;
  flex-direction: row;
  align-items: center;
}

.dropdown {
  padding-left: 1rem;

  &__select{
    height: 3rem;
    width: 100%;
    color: $color-grey;
    font-size: 1.4rem;
    border-radius: 4px;
    border: none;
    padding: 0 3rem 0 3rem;
    -webkit-appearance: none; 
    background-color: white;

    &:focus {
      outline: none !important;
      border:1px solid $color-primary;
      box-shadow: 0 0 5px #719ECE;
    }
  }
}

.POInumber {
  padding-left: 1rem;
}

.tagTitle, .descriptionTitle {
  padding: 2rem 0 1.5rem 2rem;
}

.tags {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
  padding-left: 2rem;
}

.removeButton {
  &:hover {
    cursor: pointer;
    * {
      fill: $color-primary;
    } 
  }
}

.imageContainer {
  display: flex;
  flex-direction: row;
  padding-left: 2rem;
  padding-top: 1rem;

  > img {
    width: 4rem;
    height: 4rem;
  }

  &__dimensions {
    display: flex;
    flex-direction: column;
    padding-left: 1rem;
  }

  &__formText {
    width: 6rem;
  }

  &__formRow {
    display: flex;
    flex-direction: row;
    align-items: center;
    padding-bottom: 1rem;

    > div, input {
      padding-right: 1rem;
    }

    input {
      flex:1;
      background-color: $color-grey-dark;
      border: none;
      border-radius: 5px;
      color: white;
      padding: 1rem;
      width: 5rem; 

      &::-webkit-input-placeholder {
        color:$color-grey-light;
      }

      &::-moz-placeholder {
        color:$color-grey-light;
      }

      &::-ms-placeholder {
        color:$color-grey-light;
      }

      &::placeholder {
        color:$color-grey-light;
      }

      &:focus {
        outline: none !important;
        border:1px solid $color-primary;
        box-shadow: 0 0 5px #719ECE;
      }
    }
  }
}

.divider {
  margin: 1rem 2rem 0 2rem;
  border-bottom: 1px solid #585858;
}

.description {
  &__span {
    display: block;
    overflow: hidden;
    margin: 0 2rem 0 2rem;
  }

  &__input {
    width: 100%;
    background-color: $color-grey-dark;
    padding: 1rem;
    border-radius: 4px;
  }
}

.imagesHeader, .historyHeader {
  display: flex;
  flex-direction: row;
  padding: 2rem 2rem 1.5rem 2rem;

  &__title {
    flex-grow: 1;
  }
}

.images {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
  padding: 0 2rem 0 2rem;
}

.viewMore:hover {
  color: $color-primary;
  cursor: pointer;
}

.updateHistorySmall {
  padding: 0 2rem 2rem 2rem;

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
