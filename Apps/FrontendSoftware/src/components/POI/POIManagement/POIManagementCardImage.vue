<template>
  <div>
    <img class = "POIManagementCardImage"
          :src="imageURL" 
          @click="setSelectedImage()"
          v-bind:class="{selected: this.selected}"/>
      
  </div>
</template>

<script>

import POICard from "@/data_classes/POICard.js";
import { mapGetters } from 'vuex';

export default {
  name: "POIManagementCardImage",
  props: {
    POICard: POICard,
    imageIndex: Number,
    imageURL: String
  },
  computed: {
    ...mapGetters(['POIImageSelected'])
  },
  data() {
    return {
      selected: false
    }
  },
  watch: {
    POIImageSelected: {
      deep: true, 
      handler(newObj){
        if (newObj.POICard === this.POICard && newObj.imageIndex === this.imageIndex) {
          this.selected = true;
        } else if (this.selected) {
          this.selected = false;
        }
      }
    },
  },
  methods: {
    setSelectedImage() {
      this.$store.commit("setPOIImageSelected", {POICard: this.POICard, imageIndex: this.imageIndex});
    },
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.POIManagementCardImage {
  width: 112px;
  height: 84px;
}

.selected {
  border: 1px solid $color-primary;
}

</style>
