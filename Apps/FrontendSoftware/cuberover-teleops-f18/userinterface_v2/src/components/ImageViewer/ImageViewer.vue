<!--
The highest level component for the Image Viewer. Handles layout for each element.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 5/11/2019, Colombo
-->

<template>
  <div id="imageViewer">
    <Functional class="functional"/>
    <!-- <Viewer class="viewer" /> -->
    <ImageViewport class="viewer" />
    <Toolbar class="tools"/>
    <Timeline class="timeline"/>
    <!-- MODALS -->
    <AddTag class="modal" v-if="isAddTag" v-bind:addTagName="addTagName"/>
  </div>
</template>

<script>
import ImageViewport from "@/components/ImageViewer/Viewer/ImageViewport.vue"
import Timeline from "@/components/ImageViewer/Timeline/Timeline.vue"
import Functional from "@/components/ImageViewer/functional/Functional.vue"
import Toolbar from "@/components/ImageViewer/Toolbar.vue"
//import Viewer from "@/components/ImageViewer/Viewer/Viewer.vue"
import AddTag from "@/components/ImageViewer/modals/AddTag.vue"

import { mapState } from 'vuex';

export default {
  components: {
    Timeline,
    Toolbar,
    Functional,
    ImageViewport,
    //Viewer,
    AddTag
  },
  data: function() {
    return {
      addTagName: ""
    }
  },

  methods: {
    addTag: function(payload) { // Opens add tag modial and sets addTagName to image name
      this.$store.commit('ADD_TAG')
      this.addTagName = payload
    }
  },

  computed: {
    ...mapState({
      isAddTag: state => state.IMG.isAddTag
    })
  },

  created: function() { // Adds event listners to the global event hub for adding/removing tags from an image
    this.$eventHub.$on('addTag', this.addTag)
  },

  beforeDestroy: function() { // Removes event listners from the global event hub for adding/removing tags from an image
    this.$eventHub.$off('addTag')
  }
}
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

#imageViewer {
  color: white;
  display: grid;
  grid-template-columns: 1fr min-content;
  grid-template-rows: 1fr min-content min-content;
  height: 100%;
  background-color: $color-grey-dark;
}

.functional {
  grid-area: 1 / 2 / 3 / 3;
  background-color: $color-background;
}

.tools {
  grid-area: 2 / 1 / 3 / 2;
  background-color: $color-background;
  border-right: 2px solid $color-near-black;
  border-top: 2px solid $color-near-black;
}

.timeline {
  grid-area: 3 / 1 / 4 / 4;
  background-color: $color-background;
  border-top: 2px solid $color-near-black;
}

.viewer {
  border-left: 2px solid $color-background;
  background-color: $color-near-black;
  display: flex;
  align-items: center;
  justify-content: center;
}
</style>
