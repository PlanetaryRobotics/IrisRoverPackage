<!--
Defines the Layout and Styling of the Home (main) Screen of the Iris Terminal.

Author: Connor W. Colombo, CMU
Created: 1/2019
Last Update: 07/29/2020, Colombo
-->

<template>
  <div class="main">
    <CombinedScreens>
      <ImageViewer TabName="Image Viewer" />
      <Map TabName="Map" />
      <div TabName="Telemetry" id="Telemetry" class="box"> Telemetry </div>
      <div TabName="Error Analysis" id="ErrorAnalysis" class="box"> Error Analysis </div>
      <FTA TabName="Failure Tree" ModalHeader="Failure Tree Analysis" ModalIcon="data" ref="fta" />

      <template v-slot:modal-components>
        <CLI ModalHeader="Command Line" ModalIcon="commandLine" ref="cli" />
        <div ModalHeader="Mini Map" ModalIcon="cartesianGrid" class="box" > Mini Map! </div>
      </template>
    </CombinedScreens>
  </div>

  <!--<div class="home">
    <ComponentContainer
      class='CLI'
      :header="headers.CLI"
      :fuzzyIndicator="connectionStatus"
      fuzzyIndicatorTip="Rover Connection Status"
      :menuData="[
        /*{ text:'Auto Speed Run by Command', callback: speedRun }*/
      ]"
    >
      <CLI ref="cli" />
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

  </div>-->
</template>

<script>
import { mapGetters, mapState } from 'vuex'
const { dialog } = require('electron').remote
const fs = require('fs');
const csv = require('neat-csv');

import CLI from '@/components/CLI/CLI.vue'
import ImageViewer from '@/components/ImageViewer/ImageViewer.vue'
import Map from '@/components/Map/Map.vue'

import FTA from '@/components/FTA/FTA.vue'

import CombinedScreens from '@/components/Interface/CombinedScreens.vue'

import DB from '@/DBInterface/DBInterface.js'

export default {
  name: 'home',
  components: {
    CombinedScreens,
    CLI,
    ImageViewer,
    Map,
    FTA
  },
  computed: {
    ...mapGetters({
      commandsDB: 'LogList', // List of Commands in the DB
      currentImage: 'selectedImage',
      allImages: 'images'
    }),
    ...mapState({
      commandLog: state => state.CLI.Log, // LazyList Object of Commands in the DB
      imageLog: state => state.IMG.imageList, // LazyList Object of Images in the DB
      headers: state => state.headers
    })
  },
  methods: {
    // Saves Images in the Given List of ImageData
    speedRun(){
      // Fetch Save Directory using OS Dialog:
      const files = dialog.showOpenDialog({
        properties: ['openFile'],
        message: 'Select a List of Commands',
        filters: [
          { name: 'Commands List', extensions: ['csv'] },
          { name: 'All Files', extensions: ['*'] }
        ],
        buttonLabel: "Load"
      });

      if(files && files.length > 0){
        console.log(`> Loading Commands List File from: "${files[0]}"`);
        fs.readFile(files[0], async (e, d) => {
          if(e){
            console.error(e);
            return;
          }

          let commands = await csv(d);
          console.log("- - Loaded Speed Run Data: - -");
          console.log(commands);

          let cf = this.$refs.cli.$refs.commandField;
          cf.inputText = "Speed Running...";

          let i = 0;
          let prevCommandID = -1; // lookupID of last entry in DB of when previous command was sent.
          // On first loop this will ensure that the most recent command has been sent and received a response.
          let checkForCommand = () => { // Capture the next command on the DB.
            // If there's still another command to execute:
            console.log("Checking for command...");
            if(i < commands.length){
              // Get command ID of latest entry in DB:
              let commandID = this.commandsDB[this.commandsDB.length-1].data.lookupID;

              // If Command ID is different than it was when the previous command
              // was sent, send the next command.
              console.log(commandID);
              console.log(prevCommandID);
              if(commandID != prevCommandID){ // TODO FIXME: RECURSE WITH ARGUMENT (prevCommandID)
                console.log("in");
                // Wait for Image to Come Back for the Most Recently Issued Command:
                let checkForResponseImage = () => {
                  console.log(this.allImages);
                  console.log(this.allImages[this.allImages.length - 1]);
                  console.log(this.allImages[this.allImages.length - 1].data.commandLookupID);
                  console.warn('---');

                  if(this.allImages[this.allImages.length - 1].data.commandLookupID == commandID){
                    let cmd = commands[i];
                    cf.execute(cmd.name, [cmd.amount, cmd.speed]);
                    ++i;
                    this.commandLog.onNextUpdate(checkForCommand);
                  } else {
                    imageLog.onNextUpdate(checkForResponseImage); // Check again (later).
                  }
                };
                console.log("out");
                imageLog.onNextUpdate(checkForResponseImage);
                console.log(prevCommandID);
                prevCommandID = commandID;
                console.log(prevCommandID);
                console.warn('----');
              } else {
                this.commandLog.onNextUpdate(checkForCommand); // Check again (later).
              }
            } else {
              console.log(`${i} Command Sequence Complete.`);
            }
          };
          this.commandLog.onNextUpdate( () => { checkForCommand(); });
          this.commandLog.onUpdate( () => { console.warn("UPDATE"); });
        });
      }
    },

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
  @import '@/styles/_colors.scss';
  @import '@/styles/_functional.scss';

  .main{
    height: 100vh;
    width: 100%;
    background-color: $color-near-black;
  }


  .box {
    color: $color-near-white;
    background: $color-primary;
    padding: 0.3em 0.5em;
    width: 100%;
    height: 100%;
  }
</style>
