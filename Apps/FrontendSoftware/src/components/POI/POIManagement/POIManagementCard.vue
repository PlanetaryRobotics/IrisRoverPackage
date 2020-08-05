<template>
    <div class = "POIManagementCard">
      <div class = "header" @click="toggleImages">
        <POIHeader :POIData = "POIData" class="header__title"/>

        <svg :class="{ open : show.images }" class="header__toggle" width="20" height="20" viewBox="0 0 16 16" fill="none" xmlns="http://www.w3.org/2000/svg">
          <path fill-rule="evenodd" clip-rule="evenodd" d="M14.8898 10.8798C14.7459 11.0286 14.5085 11.0327 14.3596 10.889L8.42936 5.16832C8.18715 4.93466 7.80309 4.93466 7.56087 5.16832L1.63066 10.889C1.48171 11.0327 1.24431 11.0286 1.10041 10.8798C0.956516 10.7311 0.960617 10.494 1.10957 10.3503L7.03978 4.62967C7.57265 4.11563 8.41758 4.11563 8.95045 4.62967L14.8807 10.3503C15.0296 10.494 15.0337 10.7311 14.8898 10.8798Z" fill="#FCFCFC"/>
        </svg>
      </div>

      <div class="tags">
        <div class="tags__pill" v-for="(name, index) of tagNames" :key="index">
            {{name}}
        </div>  
      </div>

      <div class = "images" v-show = "show.images">
        <div class = "images__card" v-for="(image, index) of POIData.images" :key="index">
          <img :id="generateId(image)" 
               :src="image.url"
               @click="setSelectedImage(generateId(image), image)">
          {{image.timeForTagFormatting}}
        </div>
      </div>
    </div> 
</template>

<script>

import { mapGetters } from 'vuex';
import POICard from "@/data_classes/POICard.js";
import POIHeader from "@/components/POI/Components/POIHeader.vue";

export default {
  name: "POIManagement",
  components: {
    POIHeader,
  },
  data() {
    return {
      show: {
        images: true,
      },
    }
  },
  props: {
    POIData: Object,
    POICard: POICard
  },
  computed: {
    ...mapGetters(['POIList', 'POIImageSelectedId']),
    tagNames: function() {
      let tagList = this.POIData.tagList;
      let nameList = [];
      tagList.forEach(tag => nameList.push(tag.getName()));
      return nameList;
    },
  },
  methods: {
    generateId(image) {
      // TODO: need to double check that this is effectively like a uuid
      let str = this.POICard.getName() + "-" + image.fileName;
      return str.split(' ').join('');
    },
    toggleImages() {
      this.show.images = !this.show.images;
    },
    setSelectedImage(id, image) {
      if (!this.POIImageSelectedId) {
        let elem = document.getElementById(id);
        elem.classList.add("selected");
      }
      else {
        // Remove the old ver
        let oldElem = document.getElementById(this.POIImageSelectedId);
        oldElem.classList.remove("selected");

        if (id !== this.POIImageSelectedId) {
          let elem = document.getElementById(id);
          elem.classList.add("selected");

        }
      }

      id === this.POIImageSelectedId ? 
        this.$store.commit("updatePOIImageSelectedId", {image: null, id: null}) : 
        this.$store.commit("updatePOIImageSelectedId", {image: image, id: id});
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.POIManagementCard {
  width: 100%;
  display: flex;
  flex-direction: column;
}

.images {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
  align-items: center;
  margin-bottom: 16px;

  &__card {
    padding-right: 4px;
    padding-bottom: 8px;
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;

    > img {
      width: 112px;
      height: 84px;
    }
  }
}

.header {
  display: flex;
  flex-direction: row;
  align-items: center;
  width: 100%; 
  padding-bottom: 8px;

  &__title {
    flex-grow: 1;
  }

  &__toggle {
    transform: rotate(-180deg);
    transition: .1s ease-in-out;
  }

  &:hover {
    cursor: pointer;

    .header__toggle * {
      fill: $color-primary;
    }
  }
}

.open {
  transform: rotate(0deg);
}

.tags {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
  align-items: center;
  padding-bottom: 8px;

  &__pill {
    display: flex;
    flex-direction: row;
    justify-content: center;
    padding: 4px 10px 4px 10px;
    border: 1px solid #585858;
    border-radius: 20px;
    margin-right: 0.5rem;
    margin-bottom: 1rem;
    width: auto;
  }
}

.selected {
  border: 1px solid $color-primary;
}

</style>
