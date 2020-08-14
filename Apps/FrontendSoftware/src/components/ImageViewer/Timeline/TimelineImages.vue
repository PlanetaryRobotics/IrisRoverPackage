<!--
Creates the row of images used in the timeline and controls selected image.data.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 08/14/2020, Colombo
-->

<template>
  <div>
    <!-- Iterates over list of images and creates TimelineImageCard components for each image.data. -->
    <transition-group tag="div" id="timelineImages" name="tl_trans_list" class="scrollable smooth-scroll">
      <div class="tl__cardCont" v-for="(image, index) in searchedImages" :key="image.DBID">
        <!-- UNSELECTED IMAGE CARD -->
        <div
          v-if="index != scrollPos"
          @contextmenu.prevent="() => { contextOpened(image.data.name); $refs.menu.open($event, { imgName: image.data.name }) }"
        >
          <TimelineImageCard
            class="tl_card unselected"
            :image = "image"
            :imgPos="index"
          />
        </div>
        <div
          v-if="index == scrollPos"
          @contextmenu.prevent="() => { contextOpened(image.data.name); $refs.menu.open($event, { imgName: image.data.name }) }"
        >
          <TimelineImageCard
            id="selectedTimelineCard"
            class="tl_card selected"
            :image = "image"
            :imgPos="index"
            @onchange="scrollIntoView"
          />
        </div>
        <!-- MOVE INDICATOR PRECEEDING IMAGES -->
        <div class="tl__contextCont" v-if="!index || searchedImages[index].data.commandLookupID != searchedImages[index-1].data.commandLookupID">
          <div class="tl__context">
            <img src="https://www.freeiconspng.com/uploads/white-down-arrow-png-2.png" :class="commandDirectionClass(image)">
            <span class="tl__context--text" v-html="commandStringFromImage(image)" />
          </div>
        </div>
      </div>
    </transition-group>
    <!-- CONTEXT MENU -->
    <vue-context ref="menu" id="context">
      <template slot-scope="child">
        <!-- ITERATE OVER TAGS -->
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
import { mapState, mapGetters } from 'vuex'
import { VueContext } from 'vue-context'

export default {
  components: {
    TimelineImageCard,
    VueContext
  },

  created: function() { // Adds event listners to the global event hub
    this.$eventHub.$on('IV-keypress-left', this.scrollBack);
    this.$eventHub.$on('IV-keypress-right', this.scrollForward);
  },

  beforeDestroy: function() { // Removes event listners from the global event hub
    this.$eventHub.$off('IV-keypress-left', this.scrollBack);
    this.$eventHub.$off('IV-keypress-right', this.scrollForward);
  },

  data: function() {
    return {
      tagList: []
    }
  },

  computed: {
    ...mapGetters({
      searchedImages: 'searchedImages',
      images: 'images'
    }),
    ...mapState({
      // LookupID of the Selected Image:
      commandHistory: state => state.CLI.Log.list,
      commandOptions: state => state.CLI.CommandOptions,
      selectedLookupID: state => state.IMG.selectedImageLookupID,
      scrollPos: state => state.IMG.scrollPos,
      Tags: state => state.IMG.Tags
    })
  },

  watch: {
    // Check to See if Currently Displayed Image is in the New Search Selection
    // and Scroll to it if it is:
    searchedImages: function(newVal){
      let matched = false;
      for(let i in newVal){
        if(newVal[i].data.lookupID == this.selectedLookupID){
          this.$store.state.IMG.scrollPos = Number(i);
          matched = true;
          break;
        }
      }
      if(!matched){
        this.$store.state.IMG.scrollPos = -1;
      }
    }
  },

  methods: {
    // Brings the focused selection into the center of the timeline
    scrollIntoView: function(){
      let list = document.getElementById('timelineImages');
      let item = document.getElementById('selectedTimelineCard');
      if(item != undefined && list != undefined){
        list.scrollLeft = list.scrollWidth - list.scrollWidth * (this.scrollPos + 1) / this.searchedImages.length - list.clientWidth / 2 + item.scrollWidth / 2;
      }
    },
    // Scrolls timeline's selection, on event from arrow keys
    scrollBack: function() {
      if( this.scrollPos > 0 ){
        this.$store.commit('SET_SCROLL', {pos: this.scrollPos-1, lookupID: this.searchedImages[this.scrollPos-1].data.lookupID});
      }
      this.$nextTick( () => { // Wait for DOM to update which image is highlighted,
        this.scrollIntoView(); // then scroll it into view.
      });
    },
    // Scrolls timeline's selection, on event from arrow keys
    scrollForward: function() {
      if( this.scrollPos < (this.searchedImages.length - 1) ){
        this.$store.commit('SET_SCROLL', {pos: this.scrollPos+1, lookupID: this.searchedImages[this.scrollPos+1].data.lookupID});
      }
      this.$nextTick( () => { // Wait for DOM to update which image is highlighted,
        this.scrollIntoView(); // then scroll it into view.
      });
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
    },

    // Returns a String with the Stylized Value of the First Argument (according
    // to the CommandOption spec) of the Command which Produced the Given Image.
    commandStringFromImage(img){
      let str = ""; // default
      if(img){
        let cmd = this.commandHistory.filter(c => c.data.lookupID == img.data.commandLookupID)[0];
        if(cmd){
          let commandOption = this.commandOptions.find( c => c.name == cmd.data.name );
          if(commandOption && commandOption.params.length){
            let paramIdx = Object.keys(cmd.data.args).indexOf(commandOption.params[0].name);
            str = Object.values(cmd.data.args)[paramIdx] + commandOption.formattedUnits[0];
          }
        }
      }
      return str;
    },
    // Returns the Class to the Style the Direction Indicator based on the
    // Command which Produced the Given Image.
    commandDirectionClass(img){
      let style = "tl__context--icon"; // default
      if(img){
        let cmd = this.commandHistory.filter(c => c.data.lookupID == img.data.commandLookupID)[0];
        if(cmd){
          switch(cmd.data.name){
            case "MoveForward":
              style += "-forward";
            break;
            case "MoveBackward":
              style += "-backward";
            break;
            case "TurnLeft":
              style += "-left";
            break;
            case "TurnRight":
              style += "-right";
            break;
          }
        }
      }

      return style;
    }
  }
}
</script>



<style lang="scss" scoped>
@import '@/styles/_context.scss';
@import '@/styles/_functional.scss';

$tl__animation_duration: 1s;
.tl_trans_list-enter-active, .tl_trans_list-leave-active {
  transition: all $tl__animation_duration;
}
.tl_trans_list-leave-active {
  position: absolute;
}
.tl_trans_list-enter, .tl_trans_list-leave-to {
  opacity: 0;
  transform: translateY(7rem);
}
.tl_trans_list-leave-to {
  transform: translateY(7rem);
}
.tl_trans_list-move {
  transition: transform $tl__animation_duration;
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

.smooth-scroll {
  scroll-behavior: smooth; // Prevent Abrupt Jumps
}

.tl_card {
  &.selected {
    transition: .05s ease-in-out;
    border: 1px solid $color-primary;
  }

  &.unselected {
    transition: .05s ease-in-out;
    border: 1px solid $color-background;
  }
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
    user-select: none;
  }

  &__context {
    display: flex;
    flex-direction: column;
    align-items: center;
    height: min-content;

    &--text {
      font-size: 1.1rem;
    }

    @mixin icon {
      width: 2rem;
      height: 2rem;
      background-color: $color-grey;
      border-radius: 50%;
    }

    &--icon {
      @include icon;
      transform: rotate(180deg) translate(0rem, 0rem); // default
      &-left{
        @include icon;
        transform: rotate(90deg) translate(0rem, 0rem);
      }
      &-right{
        @include icon;
        transform: rotate(270deg) translate(0rem, 0rem);
      }
      &-forward{
        @include icon;
        transform: rotate(180deg) translate(0rem, 0rem);
      }
      &-backward{
        @include icon;
        transform: rotate(0deg) translate(0rem, 0rem);
      }
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

#context{
  user-select: none;
}
</style>
