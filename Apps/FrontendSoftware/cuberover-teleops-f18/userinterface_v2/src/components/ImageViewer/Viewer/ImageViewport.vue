<!--
Displays the selected image at a large scale for navigation and allows for
editing via preset filters.

Author: Connor Colombo, CMU
Created: 3/05/2019
Last Updated: 6/3/2019, Colombo

TODO:
  - Better Denoise
  - Toggles on whether an adjustment is active
  - Reorder filters (?)
-->

<template>
  <div class="imageViewport">
    <img id="imgsrc" v-show="false" :src="selectedImage.url" alt="IMAGE NOT FOUND" @load="onImageUpdate" />
    <canvas id="imgvp" class="imgvp">
      Oops! Something went wrong and really weird. Somehow Electron doesn't support HTML5 Canvas now. What did you do?
    </canvas>
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
      type: Object,
      required: false
    }
  },

  data(){
    return {
      canvas: {},
      texture: {},
      textureInitialized: false
    };
  },

  computed: {
    ...mapState({
      scrollPos: state => state.IMG.scrollPos,
      images: state => state.IMG.images,
      // Adjustments currently being edited with sliders:
      editorAdjustments: state => state.IMG.adjustmentsEditorState.adjustments,
      presets: state => state.IMG.Presets
    }),
    // Keys of the All Adjustments:
    adjustmentKeys(){
      return Object.keys(this.editorAdjustments);
    },
    // Preset adjustment filters applied to this image:
    appliedPresets(){
      return this.presets.filter( p => p.data.global || p.data.imageList.includes(this.selectedImage.data.name) );
    },
    // Combined adjustments from the editor and the applied presets.
    totalAdjustments(){
      let tot = Object.assign({}, this.editorAdjustments); // make a copy
      this.appliedPresets.forEach(p => {
        this.adjustmentKeys.forEach(k => tot[k] += p.data.adjustments[k])
      });
      return tot;
    },
    selectedImage(){
      return this.images[this.scrollPos]
    },
    imageDOM(){
      return document.getElementById('imgsrc');
    },
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
    this.canvas = document.getElementById("imgvp");
    fx.canvas(this.canvas); // Initialize canvas for glfx
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
      if(this.canvas){ // Ensure DOM has been loaded
        try {
          // Redraw Image:
          let result = this.canvas.draw(this.texture);

          // Apply Effects:
          result = result.brightnessContrast(this.totalAdjustments.Exposure, this.totalAdjustments.Contrast)
                         .vibrance(this.totalAdjustments.Shadows);
          if(this.totalAdjustments.Denoise > 0.02 || this.totalAdjustments.Denoise < -0.02){ // slider has been moved
            result.denoise(remap(this.totalAdjustments.Denoise, -1,1, 0,75));
          }

          // Update Canvas:
          result.update();

        } catch (e) {
          console.error(e);
        }
      }
    }
  }
}
</script>

<style lang="scss" scoped>
  #imageViewport {
    user-select: none;
    display: grid;
    padding: 1rem;
    align-items: center;
  }

  .imgvp {
    width: 100%;
    grid-area: 1 / 1 / 2 / 2;

    &--overlay {
      opacity: .75;
    }
  }
</style>
