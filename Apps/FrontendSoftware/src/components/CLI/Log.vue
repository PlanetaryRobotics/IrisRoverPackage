<!--
An list of all commands sent (or attempted to be sent) to the rover.

Author: Connor W. Colombo, CMU
Created: 1/29/2019
Last Updated: 08/14/2020, Colombo
-->
<template>
  <div id="logScrollList" class="log scrollable">
    <ul>
      <li v-for="(command,index) in commands" v-bind:key="command.uuid">
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
      return this.commandsDB.concat(this.commandsLocal);
    },

    listLength(){
      return this.commands.length;
    }
  },
  watch: {
    // Called whenever commands list is updated.
    listLength(){
      // Scroll to bottom after newly added command has been given a chance to load:
      setTimeout( () => {
        let list = document.getElementById('logScrollList');
        list.scrollTop = list.scrollHeight;
      }, 200);
    }
  }
}
</script>

<style lang="scss" scoped>
  @import '@/styles/_functional.scss';
  @import '@/styles/_dimensions.scss';

  ul {
    list-style-type: none;
  }

  li {
    margin: 2 * $std-spacing;
  }
</style>
