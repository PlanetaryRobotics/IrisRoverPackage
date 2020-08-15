<template>
  <div id="toolbar">
    <div
      v-html="radialGridSVG"
      v-on:click="$store.state.IMG.radialGrid = !radialGrid; $store.state.IMG.cartesianGrid = false;"
      :class="{highlighted: radialGrid}"
      class="selectable-vector toolbar__icon tooltip" title="Polar Grid"
    />
    <div
      v-html="cartesianGridSVG"
      v-on:click="$store.state.IMG.cartesianGrid = !cartesianGrid; $store.state.IMG.radialGrid = false;"
      :class="{highlighted: cartesianGrid}"
      class="selectable-vector toolbar__icon tooltip" title="Cartesian Grid"
     />
  </div>
</template>

<script>
/* global __static */ // <- keep eslint from complaining about the __static directory
import path from 'path'
import fs from 'fs'
import { mapState } from 'vuex'

import TooltipEquip from '@/styles/TooltipEquip.js'

export default {
  name: 'Toolbar',
  props: {
  },
  data(){
    return {
      radialGridSVG: "", // Inline SVG HTML for the Radial Grid
      cartesianGridSVG: "", // Inline SVG HTML for the Cartesian Grid
    };
  },
  mounted(){
    TooltipEquip(this.$el);

    // Load Icons as Inline SVG:
    this.radialGridSVG = fs.readFileSync(path.join(__static,'./icons/icon_radial_grid.svg'), 'utf8');
    this.cartesianGridSVG = fs.readFileSync(path.join(__static,'./icons/icon_cartesian_grid.svg'), 'utf8');
  },
  computed: {
    ...mapState({
      radialGrid: state => state.IMG.radialGrid,
      cartesianGrid: state => state.IMG.cartesianGrid
    })
  }
}
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

#toolbar {
  padding: .5rem 2rem;
  display: flex;
  flex-direction: row;
}

.toolbar__icon {
  padding: 0 0.5rem;
  padding-top: 0.3rem;
}
</style>
