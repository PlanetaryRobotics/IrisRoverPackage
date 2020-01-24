<template>
  <div class="home">
    <ComponentContainer
      class='CLI'
      :header="headers.CLI"
      :fuzzyIndicator="connectionStatus"
      fuzzyIndicatorTip="Rover Connection Status"
    >
      <!-- Locations Relative to __static Directory, Found in /public -->
      <CLI />
    </ComponentContainer>
    <ComponentContainer
      class='IMG'
      :header="headers.IMG"
      :menuData="[
        { text:'Save Current Image', callback: saveCurrentImage },
        { text: 'Save All Images', callback: saveAllImages }
      ]"
    >
      <ImageViewer />
    </ComponentContainer>

  </div>
</template>

<script>
import { mapGetters, mapState } from 'vuex'
const { dialog } = require('electron').remote

import CLI from '@/components/CLI/CLI.vue'
import ImageViewer from '@/components/ImageViewer/ImageViewer.vue'
import ComponentContainer from '@/components/Interface/ComponentContainer.vue'

import DB from '@/DBInterface/DBInterface.js'

export default {
  name: 'home',
  components: {
    ComponentContainer,
    CLI,
    ImageViewer
  },
  computed: {
    ...mapGetters({
      currentImage: 'selectedImage',
      allImages: 'images'
    }),
    ...mapState({
      headers: state => state.headers
    })
  },
  methods: {
    // Saves Images in the Given List of ImageData
    saveImages(images){
      // Fetch Save Directory using OS Dialog:
      const dir = dialog.showOpenDialog({ properties: ['openDirectory'] });
      // Save All Images in Given List:
      for(const img of images){
        img.save(dir[0]);
      }
    },
    saveAllImages(){
      console.log("Saving all images . . .");
      this.saveImages(this.allImages);
      console.log("Save complete.");
    },
    saveCurrentImage(){
      this.saveImages([this.currentImage]);
    },

    /* Returns the Connection Status with the Rover.
       Right now, this returns 1 if connected to be 0 otherwise.
       TODO: This should, once specced, show 0 when not connected 0.5
       (or some fuzzy value) when just connected to DB, and 1 when connected to
       rover.
     */
    connectionStatus(){
      if(DB.connectionEstablished){
        return 1; // must be 1 not true or something truthy besides 1.
      } else{
        return 0; // must be 0  not true or something falsy besides 0.
      }
    }
  }
}
</script>

<style scoped lang="scss">
@import '@/styles/_functional.scss';
  .home{
    height: 100vh;
    width: 100%;
    display: grid;
    grid-gap: .5rem;
    grid-template-columns: 50rem 1fr;
    grid-template-rows: 1fr;
    background-color: $color-near-black;
    padding: .5rem;
  }
  .CLI {
    grid-column: 1 / 2;
    grid-row: 1 / 2;
    border-radius: 1rem;
    overflow: hidden;
  }
  .IMG {
    grid-column: 2 / 3;
    grid-row: 1 / 2;
    border-radius: 1rem;
    overflow: hidden;
  }
</style>
