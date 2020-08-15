<!--
Displays the selected image at a large scale for navigation and allows for
editing via preset filters.

Author: Connor Colombo, CMU
Created: 3/05/2019
Last Updated: 7/10/2019, Colombo

TODO:
  - Better Denoise
  - Toggles on whether an adjustment is active
  - Reorder filters (?)
-->

<template>
  <div class="image-viewport">
    <img id="imgsrc" v-show="false" :src="selectedImage.url" alt="IMAGE NOT FOUND" @load="onImageUpdate" />
    <div id="canvasContainer" style="position: relative">
      <span class="selector-handle selector-handle-nw"></span>
      <span class="selector-handle selector-handle-ne"></span>
        <canvas id="featurevp" class="port" style="z-index: 1;" />
        <!--<img class="imgvp" :v-show="radialGrid" style="z-index: 1" src="~@/assets/Overlay.png" />-->
        <canvas id="imgvp" class="port" style="z-index: 0">
          Oops! Something went wrong and really weird. Somehow Electron doesn't support HTML5 Canvas now. What did you do?
        </canvas>
      <span class="selector-handle selector-handle-se"></span>
      <span class="selector-handle selector-handle-sw"></span>
    </div>
  </div>
</template>

<script>
import { mapState } from 'vuex'
import { sha256 } from 'js-sha256'
import fx from '@/lib/glfx/glfx.js'

// Helper function to remaps the given number n from a range of (min0 to max0)
// to a range of (minf to maxf) using linear interpolation.
function remap(n, min0,max0, minf,maxf){
  return (n-min0)*(maxf-minf)/(max0-min0) + minf;
}

export default {
  name: 'ImageViewport',
  props: {
    data: {
      type: ImageData,
      required: false
    }
  },

  data(){
    return {
      canvas: {},
      texture: {},
      textureInitialized: false,
      radialGridImage: {},
      selectorState: {
        evnt: {},
        mousePos: {x: 0, y: 0}
      }
    };
  },

  computed: {
    ...mapState({
      radialGrid: state => state.IMG.radialGrid,
      // Scroll position in the timeline:
      scrollPos: state => state.IMG.scrollPos,
      // Images currently in the timeline (after search filters applied):
      images: state => state.IMG.images,
      // Adjustments currently being edited (those linked to the sliders):
      liveAdjustments: state => state.IMG.adjustmentsEditorState.adjustments,
      // All preset image filters:
      presets: state => state.IMG.Presets
    }),

    // JQuery Object for Active Viewport Region:
    JPort(){
      return $('#canvasContainer');
    },

    // Image that should be displayed in the viewport:
    selectedImage() {
      return this.images[this.scrollPos]
    },
    // Presets which should be applied to this image:
    applicablePresets(){
      return this.presets.filter( x => x.global || x.imageList.includes(selectedImage.name) );
    },
    // Superposition of all image adjustments (liveAdjustments + those from the applicablePresets):
    totalAdjustments(){
      let adj = Object.assign({}, this.liveAdjustments); // copy liveAdjustments
      let keys = Object.keys(adj);

      this.applicablePresets.forEach( p => {
        keys.forEach( a => {
          adj[a] = adj[a] + p[a];
        });
      });

      return adj;
    },
    // DOM Object of the Source Image:
    imageDOM(){
      return document.getElementById('imgsrc');
    },
    // Hash of total (combined) image adjustments:
    adjustmentsHash(){
      return sha256(JSON.stringify(this.totalAdjustments));
    }
  },
  watch: {
    adjustmentsHash: function(){
      this.applyEffects();
    }
  },

  mounted(){
    //this.canvas = document.getElementById("imgvp");
    //fx.canvas(this.canvas); // Initialize canvas for glfx

    // Add events:
    this.JPort.on('mousedown', '.selector-handle', this.resizeFeatureRegion);
  },

  methods: {
    onImageUpdate(){
      if(!this.textureInitialized){
        this.texture = this.canvas.texture(this.imageDOM); // Only create texture once
        this.textureInitialized = true;
      } else{
        this.texture.loadContentsOf(this.imageDOM);
      }
      this.applyEffects(); // Reapply effects
    },

    applyEffects(){
      try {
        // Redraw Image:
        let result = this.canvas.draw(this.texture);

        // Apply Effects:
        result = result.brightnessContrast(this.totalAdjustments.Exposure, this.totalAdjustments.Contrast)
                       .vibrance(this.totalAdjustments.Shadows);
        if(this.totalAdjustments.Denoise > 0.02 || this.totalAdjustments.Denoise < -0.02){ // slider has been moved significantly
          result.denoise(remap(this.totalAdjustments.Denoise, -1,1, 0,75));
        }

        // Update Canvas:
        result.update();

      } catch (e) {
        console.error(e);
      }
    },

    // Returns the Mouse Position within the Canvas during the Given Event.
    mousePos(e) {
      let rect = this.canvas.getBoundingClientRect();
      let x0 = (e.clientX || e.pageX || e.originalEvent.touches[0].clientX);
      let y0 = (e.clientY || e.pageY || e.originalEvent.touches[0].clientY);
      return {
          x: (x0 - rect.left) / (rect.right - rect.left) * this.canvas.width,
          y: (y0 - rect.top) / (rect.bottom - rect.top) * this.canvas.height
      };
    },

    resizeFeatureRegion(e){
      e.preventDefault();
      e.stopPropagation();

      this.saveSelectorState(e);
      alert(this.mousePos(e));
    },

    saveSelectorState(e){
      this.selectorState.evnt = e;
      this.selectorState.mousePos = this.mousePos(e);
    }/*,

    startResize(e){
      e.preventDefault();
      e.stopPropagation();

      saveEventState(e);
      this.canvas.on('mousemove', this.resize);
      this.canvas.on('mouseup', this.endResize);
    },
    resize(e){
      let width,height,left,top;
      let offset = this.canvas.offset();
      let mouse = mousePos(e);

      width = mouse.x - event_state.container_left;
      height = mouse.y  - event_state.container_top;
      left = this.canvas.container_left;
      top = this..container_top;

      if(constrain || e.shiftKey){
          height = width / orig_src.width * orig_src.height;
      }

      if(width > min_width && height > min_height && width < max_width && height < max_height){
        resizeImage(width, height);
        // Without this Firefox will not re-calculate the the image dimensions until drag end
        this.canvas.offset({'left': left, 'top': top});
      }
    },
    endResize(e){
      e.preventDefault();

      this.canvas.off('mouseup touchend', this.endResize);
      this.canvas.off('mousemove touchmove', this.resize);
    }*/
  }
}
</script>

<style lang="scss" scoped>
  @import '@/styles/_colors.scss';
  .image-viewport {
    user-select: none;
    display: grid;
    cursor: move;
    padding: 1rem;
    align-items: center;
  }
  .image-viewport img{
    display: block;
  }

  .port {
    position: absolute;
    width: 100%;
    grid-area: 1 / 1 / 2 / 2;

    &--overlay {
      opacity: .75;
    }
  }

  .port:hover,
  .port:active {
    outline: 2px dashed $color-primary;
  }

  .selector-handle-ne,
  .selector-handle-ne,
  .selector-handle-se,
  .selector-handle-nw,
  .selector-handle-sw {
      position: absolute;
      display: block;
      width: 10px;
      height: 10px;
      background: $color-primary;
      z-index: 999;
  }

  .selector-handle-nw {
      top: -5px;
      left: -5px;
      cursor: nw-resize;
  }

  .selector-handle-sw {
      bottom: -5px;
      left: -5px;
      cursor: sw-resize;
  }

  .selector-handle-ne {
      top: -5px;
      right: -5px;
      cursor: ne-resize;
  }

  .selector-handle-se {
      bottom: -5px;
      right: -5px;
      cursor: se-resize;
  }
</style>
