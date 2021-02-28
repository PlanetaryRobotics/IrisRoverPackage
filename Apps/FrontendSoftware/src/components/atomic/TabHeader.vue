<!--
Generic Tab Header for any Tabbed Paging System.

Author: Connor W. Colombo, CMU
Created: 07/12/2020
Last Update: 08/07/2020, Colombo
-->
<template>
  <div
    class="tab-header"
    :class="{ active }"
    @click="onClick"
  >
    <div class="name">
      {{ name }}
    </div>
    <div
      class="close-mark"
      :class="{ 'close-mark--hide': !closeable, 'close-mark--hide-until-hover': showCloseOnlyOnHover && closeable }"
      @click.stop="onClose" 
      v-html="CosmosIconPack('close')"
    />
  </div>
</template>

<script>
import CosmosIconPack from '@/styles/CosmosIconPack.js';

export default {
    name: 'TabHeader',
    props: {
        name: { // Name Displayed in the Tab
            type: String,
            required: true
        },
        active: { // Whether this tab rendered in an active style (i.e. its content is visible)
            type: Boolean,
            required: false,
            default: false
        },
        closeable: { // Whether this tab can be closed (i.e. the close button is rendered)
            type: Boolean,
            required: false,
            default: false
        },
        showCloseOnlyOnHover: { // Whether the close button should be hidden until hovered
            type: Boolean,
            required: false,
            default: false
        },
        onClick: { // Function called when the tab is clicked (outside of the close button)
            type: Function,
            required: false,
            default: () => {}
        },
        onClose: { // Function called when the close button is clicked
            type: Function,
            required: false,
            default: () => {}
        },
    },
    data(){
        return {
            CosmosIconPack
        };
    },
};
</script>

<style lang="scss" scoped>
    @import '@/styles/_colors.scss';
    @import '@/styles/_functional.scss';
    @import '@/styles/_dimensions.scss';
    
    $tab-height: $std-tab-height;

    $inactive-grey: transparent;
    $inactive-grey-hover: $color-grey4;
    $active-grey: $color-grey4;
    $active-grey-hover: $color-grey3;

    .tab-header {
        user-select: none;

        height: $tab-height;
        min-width: $std-tab-width;

        border-top: {
            left-radius: $std-radius;
            right-radius: $std-radius;
        };
        padding: {
            top: 1.15rem;
            bottom: $std-big-padding;
            left: $std-big-padding;
            right: $std-big-padding;
        };

        font-weight: medium;

        display: flex;
        justify-content: space-between;
        align-items: center;

        &.active {
            background-color: $active-grey;
            color: $color-near-white;
            &:hover {
                background-color: $active-grey-hover;
            }
        }
        &:not(.active) {
            background-color: $inactive-grey;
            color: $color-grey2;
            &:hover {
                background-color: $inactive-grey-hover;
                color: $color-grey1;
            }
        }
    }

    .name {
        padding-right: 1rem;
        flex-shrink: 0;
    }

    .close-mark {
        flex-shrink: 0;
        line-height: 0;
        width: 12px;
        height: 12px;

        stroke: $color-grey2;

        opacity: 1;
        transition: all 0.25s;

        /*
         * Hide and remove interactability instead of a v-show to prevent 
         * oversized tabs from shrinking when close button removed.
         */
        @mixin make-invisible() {
            pointer-events: none;
            opacity: 0;
        }
        &--hide {
            @include make-invisible();
        }

        &--hide-until-hover {
            transition: opacity 0.15s;
            &:not(:hover) {
                opacity: 0;
            }
        }

        &:hover {
            stroke: $color-near-white;
        }
    }
</style>