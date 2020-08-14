<template>
  <div class="his-item">
    <div class="his__main">
      <div class="his__main--command">{{ command }}</div>
      <div class="his__main--time">{{ timestamp }}</div>
    </div>
    <div class="his__status"></div>
  </div>
</template>

<script>
export default {
  name: "cli-history",
  props: ["item"],
  data: function() {
    return {};
  },
  computed: {
    command: function() {
      let variables = this.$store.state.commands[this.item.Name].variables;
      let name = this.$store.state.commands[this.item.Name].name;
      let title = name + " [ ";
      let tempVar = "";
      for (let variable of variables) {
        let lower = variable;
        tempVar =
          this.item[lower] + this.$store.state.variables[variable].units;
        title += tempVar + ", ";
      }
      title = title.slice(0, -2) + " ]";
      return title;
    },
    timestamp: function() {
      let seconds = this.item.Time / 1000;
      let minutes = seconds / 60;
      let hours = minutes / 60;
      seconds = Math.floor(seconds % 60);
      minutes = Math.floor(minutes % 60);
      hours = Math.floor(hours % 24) - 5;
      return hours + ":" + minutes + ":" + seconds;
    }
  }
};
</script>

<style lang="scss" scoped>
.his {
  &-item {
    color: #e5e5e5;
    text-align: left;
    margin-top: 20px;
  }
  &__main {
    display: flex;
    justify-content: space-between;
    font-weight: bold;
    background: #424242;
    border-radius: 4px 4px 0px 0px;
    padding: 10px;
    padding-left: 40px;

    &--time {
      margin-left: 70px;
      margin-right: 20px;
    }
  }
  &__status {
    padding-left: 40px;
    min-height: 4px;
    background: #67cc77;
    border-radius: 0px 0px 4px 4px;
  }
}
</style>
