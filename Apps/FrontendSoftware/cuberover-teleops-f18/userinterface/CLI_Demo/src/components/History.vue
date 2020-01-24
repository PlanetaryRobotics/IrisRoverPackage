<template>
  <div id="History">
    <cli-history
      id="history-item"
      v-for="(item, key) in this.$store.state.history"
      v-bind:key="key"
      v-bind:item="item"
    ></cli-history>
  </div>
</template>

<script>
import cliHistory from "@/components/History-Item.vue";

export default {
  name: "history",
  data: function() {
    return {};
  },
  components: {
    "cli-history": cliHistory
  }
};
</script>

<style lang="scss" scoped>
#History {
}
</style>
