<template>
    <div class="POICard">
      <!--HEADER --> 
      <div class="POICard__header">

        <POIHeader :POIData="POIData" class="POICard__headerObject"/>

        <div class="POICard__headerButtons">
          <!-- EDIT BUTTON -->
          <svg class = "POICARD__edit" width="13" height="13" viewBox="0 0 9 9" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path fill-rule="evenodd" clip-rule="evenodd" d="M6.84607 0.543934C6.78749 0.485355 6.69251 0.485355 6.63393 0.543934L5.82295 1.35492C5.76437 1.41349 5.76437 1.50847 5.82295 1.56705L6.43727 2.18136C6.69111 2.4352 6.69111 2.84676 6.43727 3.1006L1.60678 7.93109C1.55989 7.97798 1.4963 8.00431 1.43 8.00431H0V6.57431C0 6.50801 0.0263392 6.44442 0.0732233 6.39754L4.39989 2.07087C4.49752 1.97324 4.65581 1.97324 4.75344 2.07087C4.85107 2.1685 4.85107 2.32679 4.75344 2.42442L0.5 6.67787V7.50431H1.32645L6.08371 2.74705C6.14229 2.68847 6.14229 2.59349 6.08371 2.53491L5.4694 1.9206C5.21556 1.66676 5.21556 1.2552 5.4694 1.00136L6.28038 0.190381C6.53422 -0.0634599 6.94578 -0.0634605 7.19962 0.19038L7.81393 0.804695C8.06778 1.05854 8.06777 1.47009 7.81393 1.72393L7.50678 2.03109C7.40915 2.12872 7.25085 2.12872 7.15322 2.03109C7.05559 1.93346 7.05559 1.77517 7.15322 1.67754L7.46038 1.37038C7.51896 1.3118 7.51896 1.21683 7.46038 1.15825L6.84607 0.543934Z" fill="#FCFCFC"/>
          </svg>
          <!-- SHOW MORE BUTTON --> 
          <svg class = "POICard__showMore" :class="{ open : show.moreData }" @click="toggleShowMore" width="10" height="7" viewBox="0 0 8 5" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path fill-rule="evenodd" clip-rule="evenodd" d="M7.92098 4.93238C7.82026 5.02682 7.66205 5.02171 7.56761 4.92098L4.54715 1.69916C4.25085 1.3831 3.74915 1.3831 3.45285 1.69916L0.432384 4.92098C0.337951 5.02171 0.179742 5.02682 0.0790138 4.93238C-0.0217142 4.83795 -0.0268173 4.67974 0.0676146 4.57901L3.08808 1.35719C3.58192 0.830422 4.41808 0.830422 4.91192 1.35719L7.93238 4.57901C8.02682 4.67974 8.02171 4.83795 7.92098 4.93238Z" fill="#FCFCFC"/>
          </svg>
        </div>
      </div>

      <!-- MAIN BODY -->
      <div class="POICard__content">

        <!--TAGS-->
        <div class="POICard__tags">
            <div class="pill__tag" v-for="(name, index) of tagNames" :key="index">
                {{getShortName(name)}}
            </div>  
        </div>

        <div class="POICard__tags__viewMore" v-if="viewMoreTagsNumber() > 0">
              {{"+" + viewMoreTagsNumber() + ">"}}
        </div>

        <!--IMAGE AND WIDTH/HEIGHT -->
        <div class="POICard__imageRow">
          <img :src="POIData.thumbnail.url">
          <div class="POICard__imageDimensions">
            <div class="POICard__imageDimension">
              <div class="text__main--bold">
                {{"Width: "}} &nbsp;
              </div>
              {{" " + POIData.width + " " + POIData.sizeUnit}}
            </div>
            <div class="POICard__imageDimension">
              <div class="text__main--bold">
                {{"Height: "}} &nbsp;
              </div>
              {{" " + POIData.height + " " + POIData.sizeUnit}}
            </div>
          </div>
        </div>

        <!--ADDITIONAL INFO-->
        <div class="POICard__moreData" v-show = "show.moreData">
          <div class="POICard__divider" />
          <!-- HISTORY -->
          <div class="POICard__updateHistorySmall">
            <div class="POICard__updateHistorySmall--line text__small">
              <div>Created:</div> 
              <div>Helen T.</div>            <!--TODO: draw from POIData -->
              <div>10-01-2019 20:05:00</div> <!--TODO: draw from POIData -->
            </div>
            <div class="POICard__updateHistorySmall--line text__small">
              <div>Modified:</div>
              <div>Helen T.</div>            <!--TODO: draw from POIData -->
              <div>10-01-2019 20:05:00</div> <!--TODO: draw from POIData -->
            </div>
          </div>
          <!-- DESCRIPTION -->
          <div class="POICard__description">
            {{this.POIData.description}}
          </div>
          <!-- IMAGES -->
          <div class="POICard__imagesHeader">
            <div class="text__main--bold">
              Images
            </div>
            <div class="POICard__imagesViewMore">
              {{"View All (" + this.POIData.images.length + ")"}}
            </div>
          </div>
          <div class="POICard__tags">
            <div class="pill" v-for="(image, index) of images" :key="index">
              {{image.name()}}
            </div>  
          </div>
        </div> <!-- END ADDITIONAL INFO --> 
      </div> <!-- END POI CARD CONTENT --> 
    </div> <!-- END POI CARD --> 
</template>

<script>

import POICard from "@/data_classes/POICard.js";
import POIHeader from "@/components/POI/POIHeader.vue";

export default {
  name: "POICard",
  components: {
    POIHeader
  },
  data() {
    return {
      show: {
        moreData: false,
      },
    }
  },
  props: {
    POIData: Object
  },
  computed: {
    viewMoreNumbers: function() {
      return {
        tags () {
          if (this.POIData.tagList.length > 5) {
            return this.POIData.tagList.length - 5;
          } 
          return 0;
        },
        images () {
          if (this.POIData.images.length > 4) {
            return this.POIData.images.length - 4;
          } 
          return 0;
        }
      }
    },
    importanceColor: function() {
      let category = this.POIData.category;
      let ans = POICard.CATEGORY_COLORS[category];
      return {
        stroke : ans,
      };
    },
    importanceSize: function() {
      let size = this.POIData.importanceLevel;
      return new Array(size);
    },
    titleColor: function() {
      let category = this.POIData.category;
      let ans = POICard.CATEGORY_COLORS[category];
      return {
        color : ans,
      };
    },
    tagNames: function() {
      let tagList = this.POIData.tagList;
      let nameList = [];
      if (tagList.length > 7) {
        tagList.slice(0, 7).forEach(tag => nameList.push(tag.getName()));
      } else {
        tagList.forEach(tag => nameList.push(tag.getName()));
      }
      return nameList;
    },
    images: function() {
      let images = this.POIData.images;
      if (images.length > 4) {
        return images.slice(0, 4);
      }
      return images;
    },
  },
  methods: {
    getTitle() {
      if (this.POIData.category === "ATTRACTION") {
        return "ATTR-";
      } else if (this.POIData.category === "OBSTACLE") {
        return "OBST-";
      } else {
        return "SHDW-";
      }
    },
    toggleShowMore(){
      this.show.moreData = !this.show.moreData;
    },
    getShortName(name) {
      if (name.length > 7) {
        return name.substring(0, 7) + "...";
      } 
      return name;
    },
    viewMoreTagsNumber() {
      if (this.POIData.tagList.length > 7) {
        return this.POIData.tagList.length - 7;
      } 
      return 0;
    },
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.POICard {

  &__header {
    padding-top: 1rem;
    padding-right: 1rem;
    display: flex;
    flex-direction: row;
  }

  &__headerObject {
    flex-grow: 1;
  }

  &__imagesHeader {
    padding-top: 1rem;
    display: flex;
    flex-direction: row;
    justify-content: space-between;
  }

  &__imagesViewMore {
    &:hover {
      color: $color-primary;
      cursor: pointer;
    }
  }

  &__content {
    padding-left: 1rem;
    padding-right: 1rem;
  }

  &__description {
    padding-bottom: 1rem;
  }

  &__title {
    flex-grow: 1;
    padding-left: 1rem;
  }

  &__importance {
    display: flex;
    flex-direction: row;
    width: 1rem;

    > div {
      padding-right: 2px;
    }
  }

  &__headerButtons {
    display: flex;
    flex-direction: row;
    align-items: center;

    > svg {
      &:hover {
        stroke: $color-primary; 
        cursor: pointer;
      }
    }
  }

  &__showMore {
    margin-left: 1rem;
    transition: .1s ease-in-out;
  }

  &__tags {
    display: flex;
    flex-direction: row;
    flex-wrap: wrap;
    padding-top: 1rem;

    &__viewMore {
      display: flex;
      justify-content: flex-end;
      align-items: flex-start;
      margin-top: -2.5rem;
      margin-bottom: 1rem;

      &:hover {
        color: $color-primary;
        cursor: pointer;
      }
    }
  }

  &__imageRow {
    padding-top: 1rem;
    padding-bottom: 2rem;
    display: flex;
    flex-direction: row;
    align-items: center;
    justify-content: flex-start;

    > img {
      width: 4rem;
      height: 4rem;
    }
  }

  &__imageDimensions {
    display: flex;
    flex-direction: row;
    flex-grow: 1;
    justify-content: flex-start;
    flex-wrap: wrap;
  }

  &__imageDimension {
    display: flex;
    flex-direction: row;
    padding-left: 1rem;
  }

  &__moreData {
    padding-bottom: 2rem;
  }

  &__divider {
    border-bottom: 1px solid #585858;
    margin-bottom: 2rem;
  }

  &__updateHistorySmall {
    width: 80%;
    padding-bottom: 2rem;

    > div:nth-child(1) {
      padding-bottom: 1rem;
    }

    &--line {
      display: flex;
      flex-direction: row;
      justify-content: space-between;
    }
  }
}

.POIModal {
  &__images {
    position: absolute;
  }
}

$tagPillWidth: 7rem;

.pill {
  display: flex;
  flex-direction: row;
  justify-content: center;
  padding: 4px 10px 4px 10px;
  border: 1px solid #585858;
  border-radius: 20px;
  margin-right: 0.5rem;
  margin-bottom: 1rem;

  &__tag {
    @extend .pill;
    width: $tagPillWidth;
  }
}

.open {
  transform: rotate(180deg);
}

</style>
