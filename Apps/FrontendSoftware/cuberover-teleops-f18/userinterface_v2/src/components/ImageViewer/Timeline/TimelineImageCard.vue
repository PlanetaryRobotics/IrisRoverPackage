<!--
A single instance of an image as created by TimelineImages.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 3/31/2019, Moosbrugger
-->

<template>
  <div 
    class="tl__imgCont text__small"
    @click="cardClicked"
  >
    <!-- CARD TOP -->
    <div class="tl__imgCont--top">
      <p>{{ name }}</p>
      <p>{{ camera }}</p>
    </div>
    <!-- CARD IMAGE -->
    <img :src="url" :alt="name" class="tl__img">
    <!-- CARD BOTTOM -->
    <div class="tl__imgCont--bottom">
      <p>{{ time }}</p>
    </div>
  </div>
</template>

<script>

export default {
  props: {
    name: { // - Name of the image
      type: String,
      required: true
    },
    camera: { // - If the image was taken by the rover's front or back camera
      type: String,
      required: true
    },
    url: { // - Where to load the image from
      type: String,
      required: true
    },
    time: { // - The time the image was taken
      type: String,
      required: true
    },
    imgPos: { // - The index of the image in the list of images (constant for each image)
      type: Number,
      required: true
    },
  },
  methods: {
    cardClicked: function() { // If an image is clicked set that image as the scroll position
      this.$store.commit('SET_SCROLL', this.imgPos)
    }
  }
}
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
