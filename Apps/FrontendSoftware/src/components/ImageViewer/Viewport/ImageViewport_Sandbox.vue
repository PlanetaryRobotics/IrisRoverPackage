<!--
Displays the selected image at a large scale for navigation and allows for
editing via preset filters.

Author: Connor Colombo, CMU
Created: 3/05/2019
Updated: 08/30/2020, Colombo
Last Update: 10/24/2020, Gabbi LaBorwit (adding selection tool for adding new POI manually)

// TODO: fix glitch when go past top of image (priority: 1), and when off-sides how box doesn't auto border sides

-->

<template>
  <div class="image-viewport" v-on:click.self="onIVPortClick">
    <img
      class="port"
      id="imgsrc"
      v-show="false"
      :src="imageSource"
      alt="IMAGE NOT FOUND"
      @load="onImageUpdate"
    />
    <div id="portContainer"
      v-on:mousemove.stop="onMouseMove"
      v-bind:style="dragCursor"
    >
      <div
        class="canvas-wrapper port"
      >
        <div class="centering">
          <!-- Capture Science Add pop up -->
          <CaptureScienceInstructionBox
            v-if="capSciInstructionsOpen"
            :onParentClick="POISelectionInstructions"
            v-on:instructionTwoActivated="setUpCapSciSelection"
            v-on:captureSelectionSelected="onOpenCapSciModal"
          />
          <ModalCaptureScience
            v-if="capSciConfirmationModalOpen"
            v-on:closeCapSciModal="onCloseCapSciModal"
          />
        </div>

        <canvas class="port" style="z-index: 0" id="imgvp" :key="imageSource">
          Oops! Something went wrong and really weird. Somehow Electron doesn't
          support HTML5 Canvas now. What did you do?
        </canvas>
      </div>

      <!-- POI Manual Drag Select -->
      <div class="canvas-wrapper port">
        <canvas
          id="featurevp"
          class="port POIport"
          style="z-index: 1"
          v-on:mousedown.stop="onMouseDown"
          v-on:mouseup.stop="onMouseUp"
          v-bind:class="{
            crosshairMouse:
              isMouseDown &&
              !POISelectionInstructions &&
              !capSciInstructionsOpen,
          }"
        >
        </canvas>

        <!-- POI Manual Add Elements -->
        <POIModalChoiceList
          v-if="isPOIChoiceListModalVisible"
          v-on:POIChoiceSelected="onPOIChoiceSelected"
          :endCoordinates="endCoord"
        />
        <POIModalFullDetails
          :parentData="initalPOIChoiceSelected"
          v-if="arePOIFullDetailsVisible"
          v-on:closeTheModal="closePOIDetailsModal"
        />
      </div>

      <transition name="overlay">
        <img
          class="port port_overlay"
          v-if="radialGrid"
          src="~@/assets/polar_grid10.png"
        />
      </transition>
      <transition name="overlay">
        <img
          class="port port_overlay"
          v-if="cartesianGrid"
          src="~@/assets/cartesian_grid.png"
        />
      </transition>
    </div>
  </div>
</template>

<script>
const electron = require("electron");
import { mapState, mapGetters } from "vuex";
import { sha256 } from "js-sha256";
import fx from "@/lib/glfx/glfx.js";
import POIModalChoiceList from "@/components/POI/Components/POIModalChoiceList.vue";
import POIModalFullDetails from "@/components/POI/Components/POIModalFullDetails.vue";
import CaptureScienceInstructionBox from "@/components/POI/Components/CaptureScienceInstructionBox.vue";
import ModalCaptureScience from "@/components/POI/Components/ModalCaptureScience.vue";

// Helper function to remaps the given number n from a range of (min0 to max0)
// to a range of (minf to maxf) using linear interpolation.
function remap(n, min0, max0, minf, maxf) {
  return ((n - min0) * (maxf - minf)) / (max0 - min0) + minf;
}

export default {
  name: "ImageViewport",
  props: {
    data: {
      // TO-DO change name of prop
      type: ImageData,
      required: false,
    },
  },

  components: {
    POIModalChoiceList,
    POIModalFullDetails,
    CaptureScienceInstructionBox,
    ModalCaptureScience,
  },

  data() {
    return {
      portContainer: {},
      canvas: {},
      poiLayer: {},
      texture: {},
      textureInitialized: false,
      isMouseDown: false,
      isDrag: false,
      startCoord: [],
      endCoord: [],
      poiCanvasContext: null,
      fxvar: {},

      // Manual POI select vars
      isPOIChoiceListModalVisible: false,
      arePOIFullDetailsVisible: false,
      initalPOIChoiceSelected: null,

      // Capture science pop up instruction box visibility
      capSciInstructionsOpen: true, //  saving var for when commandline command entered, will turn true and start false
      POISelectionInstructions: false, // specifcally for preventing manual POI add events from occuring
      capSciConfirmationModalOpen: false,
      greenBoxTopLeftCoords: [209, 194],
      capSciExpandBoxStartCoords: [],
      capSciExpandBoxEndCoords: [],
      dragCapSciBoxActivate: false,
      dragSide: null,
      // Width of default Cap Sci selection box
      baseXOffset: 160,
      // Height of default Cap Sci selection box
      baseYOffset: 100,
      dragCursor: {
        cursor: this.setDragCursor()
      }
    };
  },

  computed: {
    ...mapGetters({
      selectedImage: "selectedImage",
    }),
    ...mapState({
      radialGrid: (state) => state.IMG.radialGrid,
      cartesianGrid: (state) => state.IMG.cartesianGrid,
      // LookupID of the Selected Image:
      lookupID: (state) => state.IMG.selectedImageLookupID,
      images: (state) => state.IMG.images,
      // Adjustments currently being edited with sliders:
      editorAdjustments: (state) =>
        state.IMG.adjustmentsEditorState.adjustments,
      presets: (state) => state.IMG.Presets,
    }),

    imageSource() {
      return this.selectedImage.failed ? "" : this.selectedImage.url;
    },

    imageDOM() {
      return document.getElementById("imgsrc");
    },

    // Keys of the All Adjustments:
    adjustmentKeys() {
      return Object.keys(this.editorAdjustments);
    },
    // Preset adjustment filters applied to this image:
    appliedPresets() {
      return this.selectedImage && this.selectedImage.data.name
        ? this.presets.filter(
            (p) =>
              p.data.global ||
              p.data.imageList.includes(this.selectedImage.data.name)
          )
        : [];
    },
    // Combined adjustments from the editor and the applied presets.
    totalAdjustments() {
      let tot = Object.assign({}, this.editorAdjustments); // make a copy
      this.appliedPresets.forEach((p) => {
        this.adjustmentKeys.forEach((k) => (tot[k] += p.data.adjustments[k]));
      });
      return tot;
    },
    adjustmentsHash() {
      // TODO: Should use flags / events on update, not this.
      return sha256(JSON.stringify(this.totalAdjustments));
    },
  },

  watch: {
    adjustmentsHash: function () {
      this.applyEffects();
    },
  },

  // runs after HTML stuff loads (part of lifecycle hooks)
  mounted() {
    this.rehookDOM();
  },

  methods: {
    onPOIChoiceSelected(val) {
      this.arePOIFullDetailsVisible = true;
      this.initalPOIChoiceSelected = val;
      this.closePOIChoiceModal();
    },

    onOpenCapSciModal() {
      // close capSciInstructionsBox
      this.capSciInstructionsOpen = false;
      //Open confirmation modal
      this.capSciConfirmationModalOpen = true;
    },

    onCloseCapSciModal() {
      this.capSciConfirmationModalOpen = false;
      this.POISelectionInstructions = false;

      //clears canvas of lines/boxes
      this.setPOILayerDimensions();
    },

    // set correct cursor icon for capture science drags
    setDragCursor(){
      if(this.dragCapSciBoxActivate){
        if (this.dragSide == "left" || this.dragSide == "right"){
          console.log("1")
          return "swResizeCursor";
        }
        else if (this.dragSide == "top" || this.dragSide == "bottom"){
          console.log("2")
          return "nsResizeCursor";
        }
      }
      console.log("3")
      return null;
    },

    setUpCapSciSelection() {
      // initialize topLeft, xOffset, and yOffset vars
      let topLeft = [];
      let topRight = [];
      let bottomRight = [];
      let bottomLeft = [];
      let xOffset = this.baseXOffset;
      let yOffset = this.baseYOffset;

      // For setting up base box before drag and adjust
      if (this.dragCapSciBoxActivate) {
        // On left side drag
        if (this.dragSide == "left") {
          topLeft = [
            this.capSciExpandBoxEndCoords[0],
            this.greenBoxTopLeftCoords[1],
          ];
          topRight = [
            this.greenBoxTopLeftCoords[0] + xOffset,
            this.greenBoxTopLeftCoords[1],
          ];
          bottomRight = [
            this.greenBoxTopLeftCoords[0] + xOffset,
            this.greenBoxTopLeftCoords[1] + yOffset,
          ];
          bottomLeft = [
            this.capSciExpandBoxEndCoords[0],
            this.greenBoxTopLeftCoords[1] + yOffset,
          ];
          // xOffset += Math.abs(this.capSciExpandBoxEndCoords[0]-this.greenBoxTopLeftCoords[0]);
        }

        // On right side drag
        else if (this.dragSide == "right") {
          topLeft = [
            this.greenBoxTopLeftCoords[0],
            this.greenBoxTopLeftCoords[1],
          ];
          topRight = [
            this.capSciExpandBoxEndCoords[0],
            this.greenBoxTopLeftCoords[1],
          ];
          bottomRight = [
            this.capSciExpandBoxEndCoords[0],
            this.greenBoxTopLeftCoords[1] + yOffset,
          ];
          bottomLeft = [
            this.greenBoxTopLeftCoords[0],
            this.greenBoxTopLeftCoords[1] + yOffset,
          ];
        }

        // On bottom side drag
        else if (this.dragSide == "bottom") {
          topLeft = [
            this.greenBoxTopLeftCoords[0],
            this.greenBoxTopLeftCoords[1],
          ];
          topRight = [
            this.greenBoxTopLeftCoords[0] + xOffset,
            this.greenBoxTopLeftCoords[1],
          ];
          bottomRight = [
            this.greenBoxTopLeftCoords[0] + xOffset,
            this.capSciExpandBoxEndCoords[1],
          ];
          bottomLeft = [
            this.greenBoxTopLeftCoords[0],
            this.capSciExpandBoxEndCoords[1],
          ];
        }

        // On bottom side drag
        else if (this.dragSide == "top") {
          topLeft = [
            this.greenBoxTopLeftCoords[0],
            this.capSciExpandBoxEndCoords[1],
          ];
          topRight = [
            this.greenBoxTopLeftCoords[0] + xOffset,
            this.capSciExpandBoxEndCoords[1],
          ];
          bottomRight = [
            this.greenBoxTopLeftCoords[0] + xOffset,
            this.greenBoxTopLeftCoords[1] + yOffset,
          ];
          bottomLeft = [
            this.greenBoxTopLeftCoords[0],
            this.greenBoxTopLeftCoords[1] + yOffset,
          ];
        }
      }
      // For setting up base box before drag and adjust
      else {
        topLeft = this.greenBoxTopLeftCoords; // x1, y1
        topRight = [topLeft[0] + xOffset, topLeft[1]]; //x2, y1
        bottomRight = [topLeft[0] + xOffset, topLeft[1] + yOffset]; // x2, y2
        bottomLeft = [topLeft[0], topLeft[1] + yOffset]; //x1, y2
      }

      // Sets POI Layer's dimensions, while also clearing canvas of any drawings currently on it
      this.setPOILayerDimensions();

      // Begin: drawing POI selector box on canvas
      this.poiCanvasContext.beginPath();

      // Style of corners
      this.poiCanvasContext.strokeStyle = "#E9E9E9";
      this.poiCanvasContext.lineWidth = 2;
      this.poiCanvasContext.globalAlpha = 1.0;

      // Draw top left corner
      this.poiCanvasContext.moveTo(topLeft[0] - 0.5, topLeft[1]);
      this.poiCanvasContext.lineTo(topLeft[0] + 11.5, topLeft[1]);
      this.poiCanvasContext.moveTo(topLeft[0], topLeft[1]);
      this.poiCanvasContext.lineTo(topLeft[0], topLeft[1] + 12);

      // Draw top right corner
      this.poiCanvasContext.moveTo(topRight[0] + 0.5, topRight[1]);
      this.poiCanvasContext.lineTo(topRight[0] - 11.5, topRight[1]);
      this.poiCanvasContext.moveTo(topRight[0], topRight[1]);
      this.poiCanvasContext.lineTo(topRight[0], topRight[1] + 12);

      //Draw bottom right corner
      this.poiCanvasContext.moveTo(bottomRight[0] + 0.5, bottomRight[1]);
      this.poiCanvasContext.lineTo(bottomRight[0] - 11.5, bottomRight[1]);
      this.poiCanvasContext.moveTo(bottomRight[0], bottomRight[1]);
      this.poiCanvasContext.lineTo(bottomRight[0], bottomRight[1] - 12);

      // Draw bottom left corner
      this.poiCanvasContext.moveTo(bottomLeft[0] - 0.5, bottomLeft[1]);
      this.poiCanvasContext.lineTo(bottomLeft[0] + 11.5, bottomLeft[1]);
      this.poiCanvasContext.moveTo(bottomLeft[0], bottomLeft[1]);
      this.poiCanvasContext.lineTo(bottomLeft[0], bottomLeft[1] - 12);

      this.poiCanvasContext.stroke();

      this.poiCanvasContext.fillStyle = "#7DE0FF";
      this.poiCanvasContext.globalAlpha = 0.35;

      let calculatedXOffset = xOffset;
      let calculatedYOffset = yOffset;
      if (this.dragCapSciBoxActivate) {
        calculatedXOffset = Math.abs(topLeft[0] - topRight[0]);
        calculatedYOffset = Math.abs(topLeft[1] - bottomRight[1]);
      }

      this.poiCanvasContext.fillRect(
        topLeft[0],
        topLeft[1],
        calculatedXOffset,
        calculatedYOffset
      );
      this.poiCanvasContext.stroke();

      // dividers
      this.poiCanvasContext.lineWidth = 1;
      this.poiCanvasContext.globalAlpha = 1.0;

      // 1/3 vertical division
      this.poiCanvasContext.moveTo(
        topRight[0] - 2 * (calculatedXOffset / 3),
        topRight[1]
      );
      this.poiCanvasContext.lineTo(
        bottomRight[0] - 2 * (calculatedXOffset / 3),
        bottomRight[1]
      );

      // 2/3 vertical division
      this.poiCanvasContext.moveTo(
        topRight[0] - calculatedXOffset / 3,
        topRight[1]
      );
      this.poiCanvasContext.lineTo(
        bottomRight[0] - calculatedXOffset / 3,
        bottomRight[1]
      );

      // 1/3 horizontal division
      this.poiCanvasContext.moveTo(
        topRight[0],
        topRight[1] + 2 * (calculatedYOffset / 3)
      );
      this.poiCanvasContext.lineTo(
        bottomLeft[0],
        topRight[1] + 2 * (calculatedYOffset / 3)
      );

      // 2/3 horizontal division
      this.poiCanvasContext.moveTo(
        topRight[0],
        topRight[1] + calculatedYOffset / 3
      );
      this.poiCanvasContext.lineTo(
        bottomLeft[0],
        topRight[1] + calculatedYOffset / 3
      );

      this.poiCanvasContext.stroke();
      this.poiCanvasContext.closePath();
    },

    // On click of page, close modals
    onIVPortClick() {
      // if not a drag-- just a click, clear canvas
      if (!this.isDrag) {
        this.setPOILayerDimensions();
        this.closePOIChoiceModal();
      } else {
        this.isDrag = false;
      }
    },

    // On mouse down in image port, draw rectangle selector and get start coordinates
    onMouseDown(event) {
      // Ignore right click
      if (event.button === 2) {
        return;
      }

      // reset end coord if already exists
      if (this.endCoord.length > 0) {
        this.endCoord = [];
      }

      this.startCoord = [event.offsetX, event.offsetY];
      this.isMouseDown = true;

      // false until proven truthy (aka don't know if drag or click until onMouseMove called or not called)
      this.isDrag = false;
      this.closePOIChoiceModal();

      if (this.POISelectionInstructions && this.capSciInstructionsOpen) {
        this.capSciExpandBoxStartCoords = this.startCoord;
      }
    },

    onMouseMove(event) {
      // If moving for Manual Add POI selection box
      if (this.isMouseDown) {
        if (!this.POISelectionInstructions && !this.capSciInstructionsOpen) {
          this.isDrag = true;
          this.endCoord = [event.offsetX, event.offsetY];
          // Ensures selection box constrained to image
          if (
            !(this.endCoord[0] <= this.poiLayer.width) ||
            !(this.endCoord[0] >= 0) ||
            !(this.endCoord[1] <= this.poiLayer.height) ||
            !(this.endCoord[1] <= this.poiLayer.width) ||
            !(this.endCoord[1] >= 0)
          ) {
            this.isMouseDown = false;

            // if cursor went past right boundary of image container
            if (this.endCoord[0] > this.poiLayer.width) {
              this.endCoord[0] = this.poiLayer.width;
            }
            // if cursor went past right boundary of image container
            else if (this.endCoord[0] < 0) {
              this.endCoord[0] = 0;
            }
            // if cursor went past top boundary of image container
            if (this.endCoord[1] > this.poiLayer.height) {
              this.endCoord[1] = this.poiLayer.height;
            }
            // if cursor went past bottom boundary of image container
            else if (this.endCoord[1] < 0) {
              this.endCoord[1] = 0;
            }
          }
          this.setUpPOISelection();
        } else if (
          this.POISelectionInstructions &&
          this.capSciInstructionsOpen
        ) {
          // dragCapSciActivated
          this.dragCapSciBoxActivate = true;

          // On left side drag
          if (
            this.capSciExpandBoxStartCoords[0] - 3 <=
              this.greenBoxTopLeftCoords[0] &&
            this.capSciExpandBoxStartCoords[0] + 3 >=
              this.greenBoxTopLeftCoords[0]
          ) {
            this.dragSide = "left";
            this.capSciExpandBoxEndCoords = [event.offsetX, event.offsetY];
            this.setUpCapSciSelection();
          }

          // On right side drag
          else if (
            this.capSciExpandBoxStartCoords[0] - 3 <=
              this.greenBoxTopLeftCoords[0] + this.baseXOffset &&
            this.capSciExpandBoxStartCoords[0] + 3 >=
              this.greenBoxTopLeftCoords[0] + this.baseXOffset
          ) {
            this.dragSide = "right";
            this.capSciExpandBoxEndCoords = [event.offsetX, event.offsetY];
            this.setUpCapSciSelection();
          }

          // On bottom side drag
          else if (
            this.capSciExpandBoxStartCoords[1] - 3 <=
              this.greenBoxTopLeftCoords[1] + this.baseYOffset &&
            this.capSciExpandBoxStartCoords[1] + 3 >=
              this.greenBoxTopLeftCoords[1] + this.baseYOffset
          ) {
            this.dragSide = "bottom";
            this.capSciExpandBoxEndCoords = [event.offsetX, event.offsetY];
            this.setUpCapSciSelection();
          }

          // On top side drag
          else if (
            this.capSciExpandBoxStartCoords[1] - 3 <=
              this.greenBoxTopLeftCoords[1] &&
            this.capSciExpandBoxStartCoords[1] + 3 >=
              this.greenBoxTopLeftCoords[1]
          ) {
            this.dragSide = "top";
            this.capSciExpandBoxEndCoords = [event.offsetX, event.offsetY];
            this.setUpCapSciSelection();
          }
        }
      }
    },

    setUpPOISelection() {
      let topLeft = this.startCoord; // x1, y1
      let topRight = [this.endCoord[0], this.startCoord[1]]; //x2, y1
      let bottomRight = this.endCoord; // x2, y2
      let bottomLeft = [this.startCoord[0], this.endCoord[1]]; //x1, y2

      // Sets POI Layer's dimensions, while also clearing canvas of any drawings currently on it
      this.setPOILayerDimensions();

      // Begin: drawing POI selector box on canvas
      this.poiCanvasContext.beginPath();

      // Style of selection box
      this.poiCanvasContext.strokeStyle = "#D5D5D5";
      this.poiCanvasContext.lineWidth = 2;

      // Start drawing selector box ("rect")
      this.poiCanvasContext.moveTo(topLeft[0], topLeft[1]);

      this.poiCanvasContext.lineTo(topRight[0], topRight[1]); // Top side of rect
      this.poiCanvasContext.lineTo(bottomRight[0], bottomRight[1]); // Right side of rect
      this.poiCanvasContext.lineTo(bottomLeft[0], bottomLeft[1]); // Bottom side of rect
      this.poiCanvasContext.lineTo(topLeft[0], topLeft[1]); // Left side of rect

      this.poiCanvasContext.stroke();
      this.poiCanvasContext.closePath();
    },

    onMouseUp() {
      this.isMouseDown = false;
      // Checks if mouse down was just a click or if mouse moved at all (i.e. selection box was formed)
      if (this.endCoord.length == 0) {
        this.startCoord = [];

        //clears canvas of lines/boxes
        if (!this.capSciInstructionsOpen) {
          this.setPOILayerDimensions();
        }

        // Checks to make sure manual POI selection box not up
        if (!this.isPOIChoiceListModalVisible && this.capSciInstructionsOpen) {
          this.POISelectionInstructions = true;
        }
      } else {
        // Show POI modal list of POI types
        this.showPOIChoiceModal();
      }

      // if box prev dragged
      if (this.dragCapSciBoxActivate) {
        this.baseXOffset = Math.abs(
          this.capSciExpandBoxEndCoords[0] -
            (this.greenBoxTopLeftCoords[0] + this.baseXOffset)
        );

        this.greenBoxTopLeftCoords = [
          this.capSciExpandBoxEndCoords[0],
          this.greenBoxTopLeftCoords[1],
        ];
      }
    },

    showPOIChoiceModal() {
      this.isPOIChoiceListModalVisible = true;
    },

    closePOIChoiceModal() {
      this.isPOIChoiceListModalVisible = false;
    },

    closePOIDetailsModal() {
      this.arePOIFullDetailsVisible = false;
      this.setPOILayerDimensions();
    },

    setPOILayerDimensions() {
      this.poiLayer.height = this.canvas.offsetHeight;
      this.poiLayer.width = this.canvas.offsetWidth;
    },

    // Update DOM Hooks:
    rehookDOM() {
      this.portContainer = document.getElementById("portContainer");
      this.poiLayer = document.getElementById("featurevp");
      if (1 || !this.canvas || !this.canvas.texture) {
        this.canvas = document.getElementById("imgvp");
        this.fxvar = fx.canvas(this.canvas); // Initialize canvas for glfx
        this.poiCanvasContext = this.poiLayer.getContext("2d");
      }
    },

    onImageUpdate() {
      this.rehookDOM();
      // clears POI canvas layer of any selection boxes
      this.setPOILayerDimensions();

      // toggles off any modals visible
      this.isPOIChoiceListModalVisible = false;
      this.capSciInstructionsOpen = true;
      this.POISelectionInstructions = false;
      this.capSciConfirmationModalOpen = false;
      this.greenBoxTopLeftCoords = [209, 194];
      this.dragCapSciBoxActivate = false;
      this.dragSide = null;
      // Width of default Cap Sci selection box
      this.baseXOffset = 160;
      // Height of default Cap Sci selection box
      this.baseYOffset = 100;

      /* First, Direct the Canvas (by changing its source image) to Have the
      Right Aspect Ratio but be way bigger than it could ever need to be - this
      prevents size capping (where a small canvas sized by a small source image
      stops growing to fit the container but the images around it continue to grow).
      Pretty hacky fix but it works with minimal cost. */
      // Fetch info about all available displays:
      let displays = electron.screen.getAllDisplays();
      // Get the largest value for vertical and horizontal size of any display:
      let maxWidth = displays.reduce(
        (max, d) => (d.bounds.width > max ? d.bounds.width : max),
        0
      );
      let maxHeight = displays.reduce(
        (max, d) => (d.bounds.height > max ? d.bounds.height : max),
        0
      );
      // Determine the largest scaling factor needed to for the image to fill the largest display:
      let maxScale = Math.max(
        maxWidth / this.imageDOM.width,
        maxHeight / this.imageDOM.height
      );
      // Direct the Size of the Image to a Size Sufficiently Larger than this Scale to Prevent Clipping:
      this.imageDOM.width = 1.5 * maxScale * this.imageDOM.width;
      this.imageDOM.height = 1.5 * maxScale * this.imageDOM.height;

      // (re)Texture the base canvas with the image:
      if (!this.textureInitialized) {
        this.texture = this.canvas.texture(this.imageDOM); // Only create texture once
        this.textureInitialized = true;
      } else {
        this.texture = this.canvas.texture(this.imageDOM); // Only create texture once
        //this.texture.loadContentsOf(this.imageDOM); // Faster to update
      }
      this.applyEffects(); // Reapply effects
    },

    applyEffects() {
      if (this.canvas && this.textureInitialized) {
        // Ensure DOM has been loaded
        try {
          // Redraw Image:
          let result = this.canvas.draw(this.texture);

          // Apply Effects:
          result = result
            .brightnessContrast(
              this.totalAdjustments.Exposure,
              this.totalAdjustments.Contrast
            )
            .vibrance(this.totalAdjustments.Shadows);
          if (
            this.totalAdjustments.Denoise > 0.02 ||
            this.totalAdjustments.Denoise < -0.02
          ) {
            // slider has been moved
            result = result.denoise(
              remap(this.totalAdjustments.Denoise, -1, 1, 0, 75)
            );
          }

          // Update Canvas:
          result.update();
        } catch (e) {
          console.error(e);
        }
      }
    },
  },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";

.image-viewport {
  padding: 2rem;
  max-width: 100%;
  max-height: 100%;
}

#portContainer {
  user-select: none;
  display: grid;
  grid-template-rows: minmax(0, 1fr);
  grid-template-columns: auto minmax(0, 1fr) auto;
  grid-template-areas: "buffa main buffb";
  align-items: center;
  justify-items: center;
  width: 100%;
  height: 100%;
}

.port {
  grid-area: main;
  object-fit: contain;
  max-height: 100%;
  max-width: 100%;
}

.canvas-wrapper {
  position: relative;
}

.centering {
  position: absolute;
  display: flex;
  justify-content: center;
  width: 100%;
  height: 100%;
}

.crosshairMouse {
  cursor: crosshair;
  // cursor: url("/assets/icons/png/cursorCrosshair.png");
}

.ewResizeCursor {
  cursor: ew-resize;
}

.nsResizeCursor{
  cursor: ns-resize;
}

/* port fade animation: */
$overlay-opacity: 0.925;
.port_overlay {
  z-index: 2;
  opacity: $overlay-opacity;
}

.overlay-enter-active {
  animation: overlay-fade-in 0.9s;
}
.overlay-leave-active {
  animation: overlay-fade-out 0.9s;
}
@keyframes overlay-fade-in {
  0% {
    opacity: 0;
  }
  100% {
    opacity: $overlay-opacity;
  }
}
@keyframes overlay-fade-out {
  /* Can't just reverse overlay-fade-in because simultaneous fade-in and
    fade-out don't work if they're both using the same keyframe. */
  0% {
    opacity: $overlay-opacity;
  }
  100% {
    opacity: 0;
  }
}
</style>
