<template>
  <div class="preset">
    <div
      v-for="(preset, index) in presetList"
      :key="index"
      class="preset__item"
    >
      <p class="preset__item--name text__main">
        {{ preset.name }}
      </p>
      <div class=" preset__item--visibility">
        <div
          title="Apply to This Image"
          :class="{highlighted: preset.visible}"
          class="selectable-vector tooltip"
        >
          <svg
            width="19"
            height="14"
            viewBox="0 0 19 14"
            fill="none"
            xmlns="http://www.w3.org/2000/svg"
            class="presetIcon"
            @click="toggleVisibility(preset.name)"
          >
            <circle
              cx="9.50019"
              cy="6.88459"
              r="2.76923"
            />
            <path d="M9.5 1C4.92308 1 1 4.92308 1 6.88462C1 8.84615 4.92308 12.7692 9.5 12.7692C14.0769 12.7692 18 8.84615 18 6.88462C18 4.92308 14.0769 1 9.5 1Z" />
          </svg>
        </div>
        <div
          title="Apply to All Images"
          :class="{highlighted: preset.global}"
          class="selectable-vector tooltip"
        >
          <svg
            width="17"
            height="17"
            viewBox="0 0 17 17"
            fill="none"
            xmlns="http://www.w3.org/2000/svg"
            class="presetIcon"
            @click="toggleGlobal(preset.name)"
          >
            <circle
              cx="8.5"
              cy="8.5"
              r="8"
            />
            <path d="M8.50016 16.5C8.32783 16.5 8.08407 16.3957 7.79001 16.0225C7.50038 15.655 7.2177 15.0923 6.97103 14.3523C6.47946 12.8776 6.16683 10.8084 6.16683 8.5C6.16683 6.19158 6.47946 4.12244 6.97104 2.64771C7.2177 1.90772 7.50038 1.34496 7.79001 0.977459C8.08407 0.604322 8.32783 0.5 8.50016 0.5C8.6725 0.5 8.91625 0.604322 9.21032 0.977459C9.49994 1.34496 9.78263 1.90772 10.0293 2.64771C10.5209 4.12244 10.8335 6.19158 10.8335 8.5C10.8335 10.8084 10.5209 12.8776 10.0293 14.3523C9.78263 15.0923 9.49994 15.655 9.21032 16.0225C8.91625 16.3957 8.6725 16.5 8.50016 16.5Z" />
            <line
              x1="1.14746"
              y1="6.81042"
              x2="15.8519"
              y2="6.81042"
              stroke-linecap="round"
            />
            <line
              x1="1.14746"
              y1="10.5883"
              x2="15.8519"
              y2="10.5883"
              stroke-linecap="round"
            />
          </svg>
        </div>
        <div
          title="Share with All Operators"
          :class="{highlighted: preset.shared}"
          class="selectable-vector tooltip"
        >
          <svg
            width="18"
            height="19"
            viewBox="0 0 18 19"
            fill="none"
            xmlns="http://www.w3.org/2000/svg"
            class="presetIcon"
          >
            <line
              x1="6.0045"
              y1="7.22018"
              x2="10.9688"
              y2="4.31807"
              stroke-linecap="round"
            />
            <circle
              cx="13.0493"
              cy="2.88185"
              r="2.38185"
            />
            <circle
              cx="3.80812"
              cy="8.45595"
              r="2.38185"
            />
            <line
              x1="5.65336"
              y1="10.0963"
              x2="10.7889"
              y2="13.0663"
              stroke-linecap="round"
            />
            <circle
              cx="13.1913"
              cy="14.1177"
              r="2.38185"
              transform="rotate(60 13.1913 14.1177)"
            />
          </svg>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import { mapState, mapGetters } from 'vuex';

import TooltipEquip from '@/styles/TooltipEquip.js';

export default {
    name: 'Presets',
    data: function() {
        return {
        };
    },
    mounted(){
        TooltipEquip(this.$el);
    },
    methods: {
        toggleVisibility: function(presetName) {
            this.$store.commit('TOGGLE_PRESET_VISIBILITY', {presetName: presetName, currentImage: this.currentImage});
        },
        toggleGlobal: function(presetName) {
            this.$store.commit('TOGGLE_PRESET_GLOBAL', presetName);
        }
    },
    computed: {
        ...mapGetters({
            currentImageData: 'selectedImage',
            images: 'images'
        }),
        ...mapState({
            Presets: state => state.IMG.Presets,
            scrollPos: state => state.IMG.scrollPos
        }),
        presetList: function() { // Creates list of presets for display
            let list = [];

            for (let index in this.Presets) { // Loops through each preset in store and extracts data
                let preset = this.Presets[index];
                list.push(preset.getData());
            }

            for (let index in list) { // Checks to see if preset is applied to current image
                if (list[index].imageList.includes(this.currentImage)) {
                    list[index].visible = true;
                } else { list[index].visible = false; }
            }
            return list;
        },

        currentImage: function() {
            return this.currentImageData ? this.currentImageData.name() : '';
        },
    },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_functional.scss";
.preset {
  padding-top: 2rem;

  &__item {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding-bottom: 1rem;

    &--visibility {
      display: flex;
      align-items: center;
    }
  }
}

.presetIcon {
  transform: scale(.8);
  margin: 0 .4rem;
}
</style>
