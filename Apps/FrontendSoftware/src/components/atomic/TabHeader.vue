<!--
Generic Tab Header for any Tabbed Paging System.

Author: Connor W. Colombo, CMU
Created: 07/12/2020
Last Update: 07/26/2020, Colombo
-->
<template>
    <div class="tab-header" :class="{ active }" @click="onClick">
        <div class="name">
            {{ name }}
        </div>
        <div v-show="closeable" class="close-mark" v-html="closeIconSVG" @click.stop="onClose" />
    </div>
</template>

<script>
/* global __static */ // <- keep eslint from complaining about the __static directory
import path from 'path'
import fs from 'fs'

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
            closeIconSVG: "", // Inline SVG HTML for the Close Button Icon
        };
    },
    mounted(){
        // Load Icons as Inline SVG:
        this.closeIconSVG = fs.readFileSync(path.join(__static,'./icons/cosmos-icons/flex/Close.svg'), 'utf8');
    }
}
</script>

<style lang="scss" scoped>
    @import '@/styles/_colors.scss';
    @import '@/styles/_functional.scss';
    @import '@/styles/_dimensions.scss';
    
    $tab-height: 3.25rem;

    $inactive-grey: transparent;
    $inactive-grey-hover: $color-grey4;
    $active-grey: $color-grey4;
    $active-grey-hover: $color-grey3;

    .tab-header {
        user-select: none;

        height: $tab-height;
        min-width: 15rem;

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

        &:hover {
            stroke: $color-near-white;
        }
    }
</style>