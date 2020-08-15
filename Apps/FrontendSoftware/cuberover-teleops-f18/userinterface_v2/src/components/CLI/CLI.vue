<!--
Simple and informative interface for sending commands to the lunar rover.

Author: Connor W. Colombo, CMU
Created: 1/30/2019
Last Update: 2/25/2019, Moosbrugger
-->
<template>
  <div class="cli">
    <Log class="command-history"/>
    <CommandField class="command-field"/>
    <queue />
  </div>
</template>

<script>
import Log from './Log.vue'
import Queue from './Queue.vue'
import CommandField from './CommandField.vue'

export default {
  name: 'CLI',
  props: {
    commands: { //          - Location of CSV File Containing Data on Valid Commands, Relative to __static Directory
      type: String,
      required: true
    },
    actionsDir: { //        - Location of Directory of JS Files for Command Actions, Relative to __static Directory
      type: String,
      required: true
    }
  },
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
