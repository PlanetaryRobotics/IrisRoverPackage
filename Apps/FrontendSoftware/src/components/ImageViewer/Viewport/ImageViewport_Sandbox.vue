<!--
Displays the selected image at a large scale for navigation and allows for
editing via preset filters.

Author: Connor Colombo, CMU
Created: 3/05/2019
Updated: 08/30/2020, Colombo
Last Update: 1/22/2020, Gabbi LaBorwit

// TODO: when off-sides how box doesn't auto border sides
-->

<template>
  <div
    class="image-viewport"
    :style="dragCursor"
    @click.self="onIVPortClick"
    @mousemove.stop="onMouseMove"
    @mousedown.stop="onMouseDown"
    @mouseup.stop="onMouseUp"
  >
    <img
      v-show="false"
      id="imgsrc"
      class="port"
      :src="imageSource"
      alt="IMAGE NOT FOUND"
      @load="onImageUpdate"
    >
    <div
      id="portContainer"
    >
      <div class="canvas-wrapper port">
        <div class="centering">
          <!-- Capture Science Add pop up -->
          <CaptureScienceInstructionBox
            v-if="capSciInstructionsOpen"
            v-show="!capSciInstructionsHidden"
            :on-parent-click="POISelectionInstructions"
            @instructionTwoActivated="setUpCapSciSelection"
            @captureSelectionSelected="onOpenCapSciModal"
          />
          <ModalCaptureScience
            v-if="capSciConfirmationModalOpen"
            @closeCapSciModal="onCloseCapSciModal"
            @confirmCapSciImage="onCapSciImageConfirmation"
          />
        </div>

        <canvas
          id="imgvp"
          :key="imageSource"
          class="port"
          style="z-index: 0"
        >
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
          :class="{
            crosshairMouse:
              isMouseDown && !POISelectionInstructions && !capSciInstructionsOpen,
          }"
          @mouseup.self="stopBubblingIfPOIModalClick"
        />

        <!-- POI Manual Add Elements -->
        <POIModalChoiceList
          v-if="isPOIChoiceListModalVisible"
          :end-coordinates="endCoord"
          @POIChoiceSelected="onPOIChoiceSelected"
        />

        <POIModalFullDetails
          v-if="arePOIFullDetailsVisible"
          :parent-data="initalPOIChoiceSelected"
          @closeTheModal="closePOIDetailsModal"
        />
      </div>

      <transition name="overlay">
        <img
          v-if="radialGrid"
          class="port port_overlay"
          draggable="false"
          src="~@/assets/polar_grid10.png"
        >
      </transition>
      <transition name="overlay">
        <img
          v-if="cartesianGrid"
          class="port port_overlay"
          draggable="false"
          src="~@/assets/cartesian_grid.png"
        >
      </transition>
    </div>
  </div>
</template>

<script>
const electron = require('electron');
import { mapState, mapGetters, mapMutations } from 'vuex';
import { sha256 } from 'js-sha256';
import fx from '@/lib/glfx/glfx.js';
import POIModalChoiceList from '@/components/POI/Components/POIModalChoiceList.vue';
import POIModalFullDetails from '@/components/POI/Components/POIModalFullDetails.vue';
import CaptureScienceInstructionBox from '@/components/POI/Components/CaptureScienceInstructionBox.vue';
import ModalCaptureScience from '@/components/POI/Components/ModalCaptureScience.vue';

// Helper function to remaps the given number n from a range of (min0 to max0)
// to a range of (minf to maxf) using linear interpolation.
function remap(n, min0, max0, minf, maxf) {
    return ((n - min0) * (maxf - minf)) / (max0 - min0) + minf;
}

export default {
    name: 'ImageViewport',

    components: {
        POIModalChoiceList,
        POIModalFullDetails,
        CaptureScienceInstructionBox,
        ModalCaptureScience,
    },
    props: {
        data: {
            // TO-DO change name of prop
            type: ImageData,
            required: false,
        },
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
            capSciInstructionsHidden: false, // to hide instructions modal when user dragging capture science box so user can see full image
            POISelectionInstructions: false, // specifically for preventing manual POI add events like selection box event from occuring
            capSciConfirmationModalOpen: false,
            // workaround for passing event array on click
            eventVarForCapSci: null,
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
                cursor: 'inherit',
            },
        };
    },

    computed: {
        ...mapGetters({
            selectedImage: 'selectedImage',
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

            // Manual POI Pop Up
            savedStartCoord: 'savedStartCoord',
            savedEndCoord: 'savedEndCoord',
        }),

        imageSource() {
            return this.selectedImage.failed ? '' : this.selectedImage.url;
        },

        imageDOM() {
            return document.getElementById('imgsrc');
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

        dragSide: function(){
            this.setDragCursor();
        }
    },

    // runs after HTML stuff loads (part of lifecycle hooks)
    mounted() {
        this.rehookDOM();
    },

    methods: {
        // Functions for POI Manual add states in Store
        ...mapMutations([
            'UPDATE_POICOORDS'
        ]),

        updatethePOICoords() {
            let POILayerDiv = document.getElementById('featurevp');

            let newStart = [this.startCoord[0]/POILayerDiv.clientWidth, this.startCoord[1]/POILayerDiv.clientHeight];

            let newEnd = [this.endCoord[0]/POILayerDiv.clientWidth, this.endCoord[1]/POILayerDiv.clientHeight];

            this.UPDATE_POICOORDS([newStart, newEnd]);
        },

        stopBubblingIfPOIModalClick(event){
            if(this.isPOIChoiceListModalVisible){
                event.stopPropagation();
            }
        },

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
            this.dragCapSciBoxActivate = false;
            this.capSciConfirmationModalOpen = false;
            this.POISelectionInstructions = false;

            //clears canvas of lines/boxes
            this.setPOILayerDimensions();
        },

        normalizeCoordinates(){
            let POILayerDiv = document.getElementById('featurevp');

            let topLeft = [];
            topLeft[0] = this.greenBoxTopLeftCoords[0]/POILayerDiv.clientWidth;
            topLeft[1] = this.greenBoxTopLeftCoords[0]/POILayerDiv.clientHeight;

            let topRight = [];
            topRight[0] = (this.greenBoxTopLeftCoords[0] + this.baseXOffset)/POILayerDiv.clientWidth;
            topRight[1] = this.greenBoxTopLeftCoords[1]/POILayerDiv.height;

            let bottomRight = [];
            bottomRight[0] = (this.greenBoxTopLeftCoords[0] + this.baseXOffset)/POILayerDiv.clientWidth;
            bottomRight[1] = (this.greenBoxTopLeftCoords[1] + this.baseYOffset)/POILayerDiv.clientHeight;

            let bottomLeft = [];
            bottomLeft[0] = this.greenBoxTopLeftCoords[0]/POILayerDiv.clientWidth;
            bottomLeft[1] = (this.greenBoxTopLeftCoords[1] + this.baseYOffset)/POILayerDiv.clientHeight;

            return [topLeft, topRight, bottomRight, bottomLeft];
        },

        onCapSciImageConfirmation(){
            //Normalize coordinates to be 0-1 (long float)
            let arr = this.normalizeCoordinates();

            let coordArr = {
                topLeft: arr[0],
                topRight: arr[1],
                bottomRight: arr[2],
                bottomLeft: arr[3]
            };

            // close Cap Sci Modal
            this.onCloseCapSciModal();

            // Send box coordinates to command line
            this.sendCapSciCoordinates(coordArr);
        },

        // set correct cursor icon for capture science drags
        setDragCursor() {
            if (this.capSciInstructionsOpen) {
                if (this.dragSide == 'left' || this.dragSide == 'right') {
                    this.dragCursor.cursor = 'ew-resize';
                } else if (this.dragSide == 'top' || this.dragSide == 'bottom') {
                    this.dragCursor.cursor = 'ns-resize';
                }
                else{
                    this.dragCursor.cursor = 'inherit';
                }
            }
            else{
                this.dragCursor.cursor = 'inherit';
            }
        },

        setUpCapSciSelection(event) {
            // initialize topLeft, xOffset, and yOffset vars
            let topLeft = [];
            let topRight = [];
            let bottomRight = [];
            let bottomLeft = [];
            let xOffset = this.baseXOffset;
            let yOffset = this.baseYOffset;
            let evt = event;

            if(!evt){
                evt = this.eventVarForCapSci;
            }

            let poiRect = document.getElementById('imgvp').getBoundingClientRect();
            let cursorInsideImageBoundary = this.checkCursorInOrOutPOILayer(evt);
            // See which side(s) cursor is closest to out of bounds if cursor out of bounds
            let sideOutOfBounds = this.checkCursorPosition(evt);

            if (this.dragCapSciBoxActivate) {
                // On left side drag
                // makes sure left side doesn't get dragged past right side, if not past right side:
                if (this.dragSide == 'left' &&
          (this.capSciExpandBoxEndCoords[0]+12) < (this.greenBoxTopLeftCoords[0] + xOffset) &&
          cursorInsideImageBoundary
                ) {
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
                }
                // if left side past right side == illegal drag
                else if (this.dragSide == 'left' && (
                    ((this.capSciExpandBoxEndCoords[0]+12) >= (this.greenBoxTopLeftCoords[0] + xOffset)) ||
          sideOutOfBounds.right
                )
                ){
                    topLeft = [
                        this.greenBoxTopLeftCoords[0] + xOffset - 12,
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
                        this.greenBoxTopLeftCoords[0] + xOffset - 12,
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                    this.capSciExpandBoxEndCoords = topLeft;
                }

                else if(this.dragSide == 'left' && !cursorInsideImageBoundary && sideOutOfBounds.left){
                    topLeft = [
                        0,
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
                        0,
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                }

                // On right side drag
                // if legal drag (i.e. right side not past left side and not past image boundary)
                // &&
                //   ((this.capSciExpandBoxEndCoords[0] - this.greenBoxTopLeftCoords[0]) < (document.getElementById("imgvp").clientWidth-this.greenBoxTopLeftCoords[0]))

                else if (this.dragSide == 'right' &&
          (this.capSciExpandBoxEndCoords[0] > (this.greenBoxTopLeftCoords[0]+12)) && cursorInsideImageBoundary
                ) {
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
                // illegal drag due to right side passing left side
                else if (this.dragSide == 'right' && (
                    (this.capSciExpandBoxEndCoords[0] <= (this.greenBoxTopLeftCoords[0] + 12)) ||
            sideOutOfBounds.left
                )
                ){
                    topLeft = [
                        this.greenBoxTopLeftCoords[0],
                        this.greenBoxTopLeftCoords[1],
                    ];
                    topRight = [
                        this.greenBoxTopLeftCoords[0] + 12,
                        this.greenBoxTopLeftCoords[1],
                    ];
                    bottomRight = [
                        this.greenBoxTopLeftCoords[0] + 12,
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                    bottomLeft = [
                        this.greenBoxTopLeftCoords[0],
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                    this.capSciExpandBoxEndCoords = topRight;
                }
                // illegal drag due to drag past image boundary
                else if(this.dragSide == 'right' && !cursorInsideImageBoundary && sideOutOfBounds.right
                ){
                    topLeft = [
                        this.greenBoxTopLeftCoords[0],
                        this.greenBoxTopLeftCoords[1],
                    ];
                    topRight = [
                        document.getElementById('imgvp').clientWidth,
                        this.greenBoxTopLeftCoords[1],
                    ];
                    bottomRight = [
                        document.getElementById('imgvp').clientWidth,
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                    bottomLeft = [
                        this.greenBoxTopLeftCoords[0],
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                    this.capSciExpandBoxEndCoords = topRight;
                }

                // On bottom side drag
                // if legal drag (i.e. bottom side not dragged past top side)
                else if(this.dragSide == 'bottom' &&
          this.capSciExpandBoxEndCoords[1] >= (this.greenBoxTopLeftCoords[1] + 12) &&
          cursorInsideImageBoundary
                ) {
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
                // illegal drag due to bottom side being dragged past top side
                else if(this.dragSide == 'bottom' && (
                    (this.capSciExpandBoxEndCoords[1] < (this.greenBoxTopLeftCoords[1] + 12)) || sideOutOfBounds.top
                )
                ){
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
                        this.greenBoxTopLeftCoords[1] + 12,
                    ];
                    bottomLeft = [
                        this.greenBoxTopLeftCoords[0],
                        this.greenBoxTopLeftCoords[1] + 12,
                    ];
                    this.capSciExpandBoxEndCoords = bottomRight;
                }

                // illegal drag due to bottom side being dragged out of image bounds
                else if(this.dragSide == 'bottom' && !cursorInsideImageBoundary && sideOutOfBounds.bottom)
                {
                    topLeft = [
                        this.greenBoxTopLeftCoords[0],
                        this.greenBoxTopLeftCoords[1]
                    ];
                    topRight = [
                        this.greenBoxTopLeftCoords[0] + xOffset,
                        this.greenBoxTopLeftCoords[1]
                    ];
                    bottomRight = [
                        this.greenBoxTopLeftCoords[0] + xOffset,
                        poiRect.bottom - poiRect.top
                    ];
                    bottomLeft = [
                        this.greenBoxTopLeftCoords[0],
                        poiRect.bottom - poiRect.top
                    ];
                }


                // On top side drag
                // if legal drag (i.e. top side not dragged past bottom side or top of image boundary)
                else if (this.dragSide == 'top' &&
          (this.capSciExpandBoxEndCoords[1] < (this.greenBoxTopLeftCoords[1] + yOffset - 12)) &&
          cursorInsideImageBoundary
                ) {
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
                // illegal drag due to top side dragged past bottom
                else if(this.dragSide == 'top' && (
                    (this.capSciExpandBoxEndCoords[1] >= (this.greenBoxTopLeftCoords[1] + yOffset - 12)) ||
            sideOutOfBounds.bottom
                )
                ){
                    topLeft = [
                        this.greenBoxTopLeftCoords[0],
                        this.greenBoxTopLeftCoords[1] + yOffset - 12,
                    ];
                    topRight = [
                        this.greenBoxTopLeftCoords[0] + xOffset,
                        this.greenBoxTopLeftCoords[1] + yOffset - 12,
                    ];
                    bottomRight = [
                        this.greenBoxTopLeftCoords[0] + xOffset,
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                    bottomLeft = [
                        this.greenBoxTopLeftCoords[0],
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                    this.capSciExpandBoxEndCoords = topLeft;
                }
                // illegal drag due to top dragged past top image boundary
                else if(this.dragSide == 'top' && !cursorInsideImageBoundary && sideOutOfBounds.top){
                    topLeft = [
                        this.greenBoxTopLeftCoords[0],
                        0,
                    ];
                    topRight = [
                        this.greenBoxTopLeftCoords[0] + xOffset,
                        0,
                    ];
                    bottomRight = [
                        this.greenBoxTopLeftCoords[0] + xOffset,
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                    bottomLeft = [
                        this.greenBoxTopLeftCoords[0],
                        this.greenBoxTopLeftCoords[1] + yOffset,
                    ];
                    this.capSciExpandBoxEndCoords = topLeft;
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
            this.poiCanvasContext.strokeStyle = '#E9E9E9';
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

            this.poiCanvasContext.fillStyle = '#7DE0FF';
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
            if (!this.isDrag && !this.dragCapSciBoxActivate) {
                this.setPOILayerDimensions();
                this.closePOIChoiceModal();
            } else {
                this.isDrag = false;
            }
        },

        checkCursorInOrOutPOILayer(e){
            // Checks if mouse currently within or on POILayer

            // Get coordinates of poi image layer
            // getBoundingClientRect returns: bottom, height, left, right, top, width, *x, y*-> don't use x, y bc not compatible w/ IE
            let domRect = document.getElementById('imgvp').getBoundingClientRect();

            // If cursor inside or on POI Layer Rect, return true
            if( (domRect.left <= e.clientX) && (e.clientX <= domRect.right)
        && (domRect.top <= e.clientY) && (e.clientY <= domRect.bottom)
            ){
                return true;
            }

            // If cursor outside of POI Layer Rect, return false
            return false;
        },

        checkCursorPosition(e){
            // Checks position of cursor outside of POI Layer box in terms of side
            let domRect = document.getElementById('imgvp').getBoundingClientRect();

            // return bool array [isOutsideOfTop, Right, Bottom, Left]
            return {
                top: (e.clientY < domRect.top),
                right: (domRect.right < e.clientX),
                bottom: (domRect.bottom < e.clientY),
                left: (e.clientX < domRect.left)
            };
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

            // false until proven truthy (aka don't know if drag or click until onMouseMove called or not called)
            this.isDrag = false;

            // If cursor inside POI Layer, set start coordinate of selection box
            if (this.checkCursorInOrOutPOILayer(event)) {
                this.startCoord = [event.offsetX, event.offsetY];
                this.isMouseDown = true;
            }

            if (this.POISelectionInstructions && this.capSciInstructionsOpen) {
                this.capSciExpandBoxStartCoords = this.startCoord;
            }
        },

        onMouseMove(event) {
            // let eventX = event.clientX
            // let eventY = event.clientY-47;
            // If moving while mouse down, for cap sci and selection box logic
            // if (this.isMouseDown) {
            //   if (!this.POISelectionInstructions && !this.capSciInstructionsOpen && !this.arePOIFullDetailsVisible) {
            if (this.isMouseDown) {

                let dragDist = Math.sqrt((event.offsetX - this.startCoord[0])**2 + (event.offsetY - this.startCoord[1])**2);

                if (!this.POISelectionInstructions && !this.capSciInstructionsOpen && !this.arePOIFullDetailsVisible && dragDist > 19) {
                    this.isDrag = true;

                    this.endCoord = [event.offsetX, event.offsetY];

                    this.setPOILayerDimensions();
                    this.closePOIChoiceModal();

                    // Ensures POI selection box constrained to POI image Layer
                    if (!this.checkCursorInOrOutPOILayer(event)) {

                        // Get which sides of POI Layer rect cursor is closest to
                        let cursorSidePositions = this.checkCursorPosition(event);

                        // if cursor went past right boundary of POI image container
                        if (cursorSidePositions.right) {
                            this.endCoord[0] = this.poiLayer.width;
                        }

                        // if cursor went past left boundary of POI image container
                        if (cursorSidePositions.left) {
                            this.endCoord[0] = 0;
                        }

                        // if cursor went past top boundary of POI image container
                        if (cursorSidePositions.top) {
                            // If cursor is not also outside of left or right side, set x coord to not offset
                            if(!cursorSidePositions.left && !cursorSidePositions.right){
                                // gets x coordinate relative to POI Layer
                                this.endCoord[0] = event.clientX - document.getElementById('imgvp').getBoundingClientRect().left;
                            }
                            this.endCoord[1] = 0;
                        }

                        // if cursor went past bottom boundary of POI image container
                        if (cursorSidePositions.bottom) {
                            // If cursor is not also outside of left or right side, set x coord to not offset
                            let poiRect = document.getElementById('imgvp').getBoundingClientRect();

                            if(!cursorSidePositions.left && !cursorSidePositions.right){
                                // gets x coordinate relative to POI Layer
                                this.endCoord[0] = event.clientX - poiRect.left;
                            }
                            this.endCoord[1] = poiRect.bottom - poiRect.top;
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
                        // close Instructions while dragging so user can see full image
                        this.capSciInstructionsHidden = true;
                        this.dragSide = 'left';
                        this.capSciExpandBoxEndCoords = [event.offsetX, event.offsetY];
                        this.setUpCapSciSelection(event);
                    }

                    // On right side drag
                    else if (
                        this.capSciExpandBoxStartCoords[0] - 3 <=
              this.greenBoxTopLeftCoords[0] + this.baseXOffset &&
            this.capSciExpandBoxStartCoords[0] + 3 >=
              this.greenBoxTopLeftCoords[0] + this.baseXOffset
                    ) {
                        // close Instructions while dragging so user can see full image
                        this.capSciInstructionsHidden = true;
                        this.dragSide = 'right';
                        this.capSciExpandBoxEndCoords = [event.offsetX, event.offsetY];
                        this.setUpCapSciSelection(event);
                    }

                    // On bottom side drag
                    else if (
                        this.capSciExpandBoxStartCoords[1] - 3 <=
              this.greenBoxTopLeftCoords[1] + this.baseYOffset &&
            this.capSciExpandBoxStartCoords[1] + 3 >=
              this.greenBoxTopLeftCoords[1] + this.baseYOffset
                    ) {
                        // close Instructions while dragging so user can see full image
                        this.capSciInstructionsHidden = true;
                        this.dragSide = 'bottom';
                        this.capSciExpandBoxEndCoords = [event.offsetX, event.offsetY];
                        this.setUpCapSciSelection(event);
                    }

                    // On top side drag
                    else if (
                        this.capSciExpandBoxStartCoords[1] - 3 <=
              this.greenBoxTopLeftCoords[1] &&
            this.capSciExpandBoxStartCoords[1] + 3 >=
              this.greenBoxTopLeftCoords[1]
                    ) {
                        // close Instructions while dragging so user can see full image
                        this.capSciInstructionsHidden = true;
                        this.dragSide = 'top';
                        this.capSciExpandBoxEndCoords = [event.offsetX, event.offsetY];
                        this.setUpCapSciSelection(event);
                    }
                }
            }
            // When mouse not down but hovering over cap sci box sides, change cursor
            else{
                // hover over left side of selection box
                if (
                    event.offsetX - 3 <=
              this.greenBoxTopLeftCoords[0] &&
            event.offsetX + 3 >=
              this.greenBoxTopLeftCoords[0]
                ) {
                    this.dragSide = 'left';
                }

                // hover over right side of cap sci selection box
                else if (
                    event.offsetX - 3 <=
              this.greenBoxTopLeftCoords[0] + this.baseXOffset &&
            event.offsetX + 3 >=
              this.greenBoxTopLeftCoords[0] + this.baseXOffset
                ) {
                    this.dragSide = 'right';
                }

                // hover over bottom side of cap sci selection box
                else if (
                    event.offsetY - 3 <=
              this.greenBoxTopLeftCoords[1] + this.baseYOffset &&
            event.offsetY + 3 >=
              this.greenBoxTopLeftCoords[1] + this.baseYOffset
                ) {
                    this.dragSide = 'bottom';
                }

                // hover over top side of cap sci selection box
                else if (
                    event.offsetY - 3 <=
              this.greenBoxTopLeftCoords[1] &&
            event.offsetY + 3 >=
              this.greenBoxTopLeftCoords[1]
                ) {
                    this.dragSide = 'top';
                }

                else{
                    this.dragSide = null;
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
            this.poiCanvasContext.strokeStyle = '#D5D5D5';
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

        onMouseUp(event) {
            this.isMouseDown = false;
            // Checks if mouse down was just a click or if mouse moved at all (i.e. selection box was formed)
            // if (this.endCoord.length == 0) {
            if (!this.isDrag) {
                this.startCoord = [];

                //clears canvas of lines/boxes if single click on screen and not in POI Choices box
                if (!this.isPOIChoiceListModalVisible && !this.capSciInstructionsOpen && !this.capSciConfirmationModalOpen && !this.arePOIFullDetailsVisible) {
                    this.setPOILayerDimensions();
                }

                // Checks to make sure manual POI selection box not up
                if (!this.isPOIChoiceListModalVisible && this.capSciInstructionsOpen) {
                    this.POISelectionInstructions = true;
                    this.eventVarForCapSci = event;
                }

                if(this.isPOIChoiceListModalVisible){
                    // this.setPOILayerDimensions();
                    this.closePOIChoiceModal();
                }

            } else if(this.isDrag){
                // Update coords in store state
                this.updatethePOICoords();
                // Show POI modal list of POI types
                this.showPOIChoiceModal();
            }

            // if box prev dragged, set update default box coordinates: offsets
            if (this.dragCapSciBoxActivate) {
                if (this.dragSide == 'left') {
                    this.baseXOffset = Math.abs(
                        this.capSciExpandBoxEndCoords[0] -
              (this.greenBoxTopLeftCoords[0] + this.baseXOffset)
                    );
                } else if (this.dragSide == 'right') {
                    this.baseXOffset = Math.abs(
                        this.capSciExpandBoxEndCoords[0] - this.greenBoxTopLeftCoords[0]
                    );
                } else if (this.dragSide == 'top') {
                    this.baseYOffset = Math.abs(
                        this.capSciExpandBoxEndCoords[1] -
              (this.greenBoxTopLeftCoords[1] + this.baseYOffset)
                    );
                } else if (this.dragSide == 'bottom') {
                    this.baseYOffset = Math.abs(
                        this.capSciExpandBoxEndCoords[1] - this.greenBoxTopLeftCoords[1]
                    );
                }

                // update default box coordinates to new box coords
                if (this.dragSide == 'left') {
                    this.greenBoxTopLeftCoords = [
                        this.capSciExpandBoxEndCoords[0],
                        this.greenBoxTopLeftCoords[1],
                    ];
                } else if (this.dragSide == 'top') {
                    this.greenBoxTopLeftCoords = [
                        this.greenBoxTopLeftCoords[0],
                        this.capSciExpandBoxEndCoords[1],
                    ];
                }
                this.dragSide = null;
                this.capSciInstructionsHidden = false;
                this.capSci;
            }
        },

        // Command to send coordinates to rover
        sendCapSciCoordinates(coords){
            // Object is array of coords with following keys:
            /* coords = {
          topLeft, topRight, bottomRight, bottomLeft
         }
      (e.g. coords.topLeft or coords.bottomLeft)
      */

            console.log('coords: ', coords);
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
            this.portContainer = document.getElementById('portContainer');
            this.poiLayer = document.getElementById('featurevp');
            if (!this.canvas || !this.canvas.texture) {
                this.canvas = document.getElementById('imgvp');
                this.fxvar = fx.canvas(this.canvas); // Initialize canvas for glfx
                this.poiCanvasContext = this.poiLayer.getContext('2d');
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
