<!--
Simple and informative interface for sending commands to the lunar rover.

Author: Connor W. Colombo, CMU
Created: 1/30/2019
Last Update: 7/31/2020, Colombo
-->
<template>
  <div class="cli">
    <Log class="command-history" />
    <CommandField class="command-field" ref="commandField" />
    <queue />
  </div>
</template>

<script>
import Log from './Log.vue'
import Queue from './Queue.vue'
import CommandField from './CommandField.vue'

export default {
  name: 'CLI',
  props: {},
  components: {
    Log,
    Queue,
    CommandField
  }
}
</script>

<style lang="scss" scoped>
  @import '@/styles/_functional.scss';

  .cli {
    position: relative;
    display: grid;
    grid-template-rows: 1fr min-content;
    height: 100%;
    background-color: $color-background;
  }
  .command-history {
    overflow: auto;
  }
</style>
