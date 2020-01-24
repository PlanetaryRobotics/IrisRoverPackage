<!--
Creates the row of images used in the timeline and controls selected image.data.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 4/14/2019, Moosbrugger
-->

<template>
  <div id="timelineImages">
    <!-- Iterates over list of images and creates TimelineImageCard components for each image.data. -->
    <div class="tl__cardCont" v-for="(image, index) in searchedImages" :key="index">
      <!-- SELECTED IMAGE CARD -->
      <div
        v-if="index === scrollPos"
        @contextmenu.prevent="() => { contextOpened(image.data.name); $refs.menu.open($event, { imgName: image.data.name }) }"
      >
        <TimelineImageCard
          class="selected"
          v-bind:name="image.data.name"
          v-bind:camera="image.data.camera"
          v-bind:url="image.url"
          v-bind:time="image.time"
          v-bind:imgPos="index"
        />
      </div>
      <!-- UNSELECTED IMAGE CARD -->
      <div
        v-if="index != scrollPos"
        @contextmenu.prevent="() => { contextOpened(image.data.name); $refs.menu.open($event, { imgName: image.data.name }) }"
      >
        <TimelineImageCard
          class="unselected"
          v-bind:name="image.data.name"
          v-bind:camera="image.data.camera"
          v-bind:url="image.url"
          v-bind:time="image.time"
          v-bind:imgPos="index"
        />
      </div>
      <!-- MOVE INDICATOR PRECEEDING IMAGES -->
      <div class="tl__contextCont">
        <div class="tl__context">
          <img src="https://www.freeiconspng.com/uploads/white-down-arrow-png-2.png" class="tl__context--icon">
          <p class="tl__context--text">10cm</p>
        </div>
      </div>
    </div>
    <!-- CONTEXT MENU -->
    <vue-context ref="menu" id="context">
      <template slot-scope="child">
        <!-- ITERATES OVER TAGS -->
        <div v-for="(tag, index) in tagList" :key="index">
          <p class="context__option" @click="tagToggle(tag.tagName, child.data.imgName)">
            {{ tag.tagName }}
            <svg width="8" height="9" viewBox="0 0 8 9" fill="none" xmlns="http://www.w3.org/2000/svg" :class="{tagIndicator__applied: tag.isApplied}" class="tagIndicator">
              <path d="M0.5 6C1.08582 6.39055 1.67164 6.93362 2.16811 7.45051C2.74454 8.05067 3.84824 7.83969 4.13193 7.05739C4.7956 5.22728 5.95744 2.54256 7.5 1" stroke-linecap="round"/>
            </svg>
          </p>
        </div>
        <hr class="context__divider" v-if="tagList.length > 0">
        <!-- PERMANENT MENU -->
        <div class="context__option" @click="addTagsClicked(child.data.imgName)">
          <p>Add New Tag</p>
          <svg width="17" height="17" viewBox="0 0 17 17" fill="none" xmlns="http://www.w3.org/2000/svg" class="context__option--icon">
            <line x1="8.5" y1="4.5" x2="8.5" y2="12.5" stroke-linecap="round" stroke-linejoin="round"/>
            <line x1="4.5" y1="8.5" x2="12.5" y2="8.5" stroke-linecap="round" stroke-linejoin="round"/>
            <circle cx="8.5" cy="8.5" r="8"/>
          </svg>
        </div>
        <div class="context__option" @click="manageTagsClicked(child.data.imgName)">
          <p>Manage Tags</p>
          <svg width="17" height="17" viewBox="0 0 17 17" fill="none" xmlns="http://www.w3.org/2000/svg" class="context__option--icon">
            <line x1="4.5" y1="8.5" x2="12.5" y2="8.5" stroke-linecap="round" stroke-linejoin="round"/>
            <circle cx="8.5" cy="8.5" r="8"/>
          </svg>
        </div>
      </template>
    </vue-context>
  </div>
</template>



<script>
import TimelineImageCard from "@/components/ImageViewer/Timeline/TimelineImageCard.vue"
import { mapState } from 'vuex';
import { VueContext } from 'vue-context'

export default {
  components: {
    TimelineImageCard,
    VueContext
  },

  created: function() { // Adds event listners to the global event hub
    this.$eventHub.$on('leftArrowListener', this.scrollBack)
    this.$eventHub.$on('rightArrowListener', this.scrollForward)
  },

  beforeDestroy: function() { // Removes event listners from the global event hub
    this.$eventHub.$on('leftArrowListener')
    this.$eventHub.$on('rightArrowListener')
  },

  data: function() {
    return {
      tagList: []
    }
  },

  computed: {
    scrollList: function () { // Creates the list of images to be displayed.
      return this.images.slice(0, 100);
    },

    searchedImages: function() {
      return this.$store.getters.searchedImages
    },

    ...mapState({
      scrollPos: state => state.IMG.scrollPos,
      images: state => state.IMG.images,
      Tags: state => state.IMG.Tags,
    })
  },

  methods: {
    scrollBack: function() { // Scrolls timeline on event from arrow keys
      if ( this.scrollPos < (this.images.length - 1)) {
        this.$store.commit('SCROLL_BACK')
      }
    },

    scrollForward: function() { // Scrolls timeline on event from arrow keys
      if ( this.scrollPos > 0) {
        this.$store.commit('SCROLL_FORWARD')
      }
    },

    tagToggle(tag, imgName) { // Output of context menu.
      this.$store.commit('TOGGLE_TAG', {tagName: tag, imgName: imgName})
    },

    addTagsClicked(imgName) { // Emits event when 'add tag' is clicked for AddTag window to appear
      this.$eventHub.$emit('addTag', imgName)
    },

    manageTagsClicked(imgName) { // Emits event when 'manage tag' is clicked for ManageTags window to appear
      alert('Open Manage Tags Menu', imgName)
    },

    contextOpened(clickedImage) {
      let list = []
      for (const tag of this.Tags) {
        if (tag.getImages().includes(clickedImage)) {
          list.push({tagName: tag.getName(), isApplied: true})
        }
        else {
          list.push({tagName: tag.getName(), isApplied: false})
        }
      }
      this.tagList = list
    }
  }
}
</script>



<style lang="scss" scoped>
@import '@/styles/_functional.scss';

.selected {
  transition: .05s ease-in-out;
  border: 1px solid $color-primary;
}
.unselected {
  transition: .05s ease-in-out;
  border: 1px solid $color-background;
}

#timelineImages {
  width: 100%;
  display: flex;
  flex-direction: row-reverse;
  overflow-x: scroll;
  overflow-y: hidden;
  padding-bottom: 1.5rem;
  min-width: 0;
}

.tl {
  &__cardCont {
    flex-shrink: 0;
    display: grid;
    grid-template-columns: 1fr min-content;
    margin-right: 1rem;
  }

  &__contextCont {
    position: relative;
    display: flex;
    align-items: center;
    margin-left: 1rem;
  }

  &__context {
    display: flex;
    flex-direction: column;
    align-items: center;
    height: min-content;

    &--text {
      font-size: 1.1rem;
    }

    &--icon {
      width: 2rem;
      height: 2rem;
      transform: rotate(90deg) translate(-.2rem, .1rem);
      background-color: $color-grey;
      border-radius: 50%;
    }
  }
}

.tagIndicator {
  stroke: $color-background;
  transform: scale(1.2);

  &__applied {
    stroke: $color-near-white;
  }
}
</style>
