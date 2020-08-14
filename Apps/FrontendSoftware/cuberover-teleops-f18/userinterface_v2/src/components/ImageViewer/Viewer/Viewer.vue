<!--
Displays the selected image at a large scale for navigation.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 3/31/2019, Moosbrugger
-->
<template>
  <div id="viewer">
    <img :src="selectedImage.url" alt="IMAGE NOT FOUND" class="viewer__img">
  </div>
</template>

<script>
import { mapState } from 'vuex'

export default {
  computed: {
    ...mapState({
      scrollPos: state => state.IMG.scrollPos,
      images: state => state.IMG.images
    }),

    selectedImage() {
      return this.images[this.scrollPos]
    }
  }
}
</script>

<style lang="scss" scoped>
#viewer {
  user-select: none;
  display: grid;
  padding: 1rem;
  align-items: center;
}

.viewer__img {
  width: 100%;
  grid-area: 1 / 1 / 2 / 2;

  &--overlay {
    opacity: .75;
  }
}
</style>
