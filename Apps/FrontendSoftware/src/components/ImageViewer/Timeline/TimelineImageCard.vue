<!--
A single instance of an image as created by TimelineImages.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 10/7/2019, Colombo
-->

<template>
  <div
    class="tl__imgCont text__small"
    @click="cardClicked"
  >
    <!-- CARD TOP -->
    <div class="tl__imgCont--top">
      <p>{{ image.data.name }}</p>
      <p>{{ image.data.camera }}</p>
    </div>
    <!-- CARD IMAGE -->
    <img
      :key="image.data.lookupID"
      :src="image.url"
      :alt="image.data.name"
      class="tl__img"
    >
    <!-- CARD BOTTOM -->
    <div class="tl__imgCont--bottom">
      <p>{{ image.time }}</p>
      <p>{{ image.data.lookupID }}</p>
    </div>
  </div>
</template>

<script>
import ImageData from '@/data_classes/ImageData.js';

export default {
    props: {
        image: {
            type: ImageData,
            required: true
        },
        imgPos: { // - The index of the image in the list of images (constant for each image)
            type: Number,
            required: true
        },
    },
    methods: {
        cardClicked: function() { // If an image is clicked set that image as the scroll position
            this.$store.commit('SET_SCROLL', {pos: this.imgPos, lookupID: this.image.data.lookupID});
        }
    }
};
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

.tl {
  &__imgCont {
    display: grid;
    grid-template-rows: min-content 1fr min-content;
    background-color: $color-grey-dark;
    border-radius: 1rem;
    overflow: hidden;
    user-select: none;

    &--top {
      display: flex;
      justify-content: space-between;
      padding: .5rem 1rem;
      z-index: 10;
      grid-area: 1 / 1 / 2 / 2;
      background-color: rgba(40, 40, 40, .5);
    }

    &--bottom {
      display: flex;
      justify-content: space-between;
      padding: .5rem 1rem;
      z-index: 10;
      background-color: rgba(40, 40, 40, .5);
      grid-area: 3 / 1 / 4 / 2;
    }
  }

  &__img {
    width: 20rem;
    filter: grayscale(1);
    grid-area: 1 / 1 / 4 / 2;
  }
}
</style>
