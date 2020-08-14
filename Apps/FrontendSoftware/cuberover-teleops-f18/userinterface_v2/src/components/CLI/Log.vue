<!--
An list of all commands sent (or attempted to be sent) to the rover.

Author: Connor W. Colombo, CMU
Created: 1/29/2019
Last Updated: 4/26/2019, Colombo
-->
<template>
  <div class="log scrollable">
    <ul>
      <li v-for="(command,index) in commands" v-bind:key="index">
        <Command v-bind:ref="`command${index}`" v-bind:core="command" />
      </li>
    </ul>
  </div>
</template>

<script>
import { mapState, mapGetters } from 'vuex'

import Command from './Command.vue'

export default {
  name: 'Log',
  components: {
    Command
  },
  computed: {
    ...mapGetters({
      commandsDB: 'LogList', // Getter FUNCTION for CommandData Objects on the Database from DBLazyList
    }),
    ...mapState({
      commandsLocal: state => state.CLI.LogWaiting // Array of CommandData Objects waiting to be pushed to database.
    }),

    // List of all commands issued
    commands(){
      return this.commandsDB().concat(this.commandsLocal);
    }
  }
}
</script>

<style lang="scss" scoped>
  @import '@/styles/_functional.scss';

  .log {
    height: 86vh;
  }

  ul {
    list-style-type: none;
  }
</style>
