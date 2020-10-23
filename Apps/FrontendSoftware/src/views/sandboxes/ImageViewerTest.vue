<!--
The highest level component for the Image Viewer. Handles layout for each element.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 08/13/2020, Colombo
-->

<template>
  <div 
    id="imageViewer"
    tabindex="0 /*this allows the container to capture keyevents from anywhere inside it*/"
    @keyup.right="$eventHub.$emit('IV-keypress-right')"
    @keyup.left="$eventHub.$emit('IV-keypress-left')"
  >
    <Functional class="functional"/>
    <ImageViewport class="viewer" />
    <Toolbar class="tools"/>
    <Timeline class="timeline"/>
    <!-- MODALS -->
    <AddTag class="modal" v-if="isAddTag" v-bind:addTagName="addTagName"/>
  </div>
</template>

<script>
import ImageViewport from "@/components/ImageViewer/Viewport/ImageViewport_Sandbox.vue"
import Timeline from "@/components/ImageViewer/Timeline/Timeline.vue"
import Functional from "@/components/ImageViewer/functional/Functional.vue"
import Toolbar from "@/components/ImageViewer/Toolbar.vue"
import AddTag from "@/components/ImageViewer/modals/AddTag.vue"

import { mapState } from 'vuex';

export default {
  components: {
    Timeline,
    Toolbar,
    Functional,
    ImageViewport,
    AddTag
  },
  data: function() {
    return {
      addTagName: "",
      console
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
    this.$eventHub.$on('addTag', this.addTag);
  },

  beforeDestroy: function() { // Removes event listners from the global event hub for adding/removing tags from an image
    this.$eventHub.$off('addTag', this.addTag);
  }
}
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

#imageViewer {
  height: 100%;
  max-width: 100%;
  position: relative;
  display: grid;
  grid-template-columns: minmax(0, 1fr) min-content;
  grid-template-rows: minmax(0, 1fr) min-content min-content;
  grid-template-areas:
    "port func"
    "tool func"
    "time time";
  background-color: $color-grey-dark;
  color: white;

  > .viewer {
    grid-area: port;
    border-left: 2px solid $color-background;
    background-color: $color-near-black;
    display: flex;
    align-items: center;
    justify-content: center;
  }

  > .functional {
    grid-area: func;
    background-color: $color-background;
  }

  > .tools {
    grid-area: tool;
    background-color: $color-background;
    border-right: 2px solid $color-near-black;
    border-top: 2px solid $color-near-black;
    overflow: hidden;
  }

  .timeline {
    grid-area: time;
    background-color: $color-background;
    border-top: 2px solid $color-near-black;
  }
}


</style>
