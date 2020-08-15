<template>
  <div class="home">
    <cli-history id="cli-history"/>
    <cli-input id="cli-inputed"/>
  </div>
</template>

<script>
// @ is an alias to /src
import cliInput from "@/components/input.vue";
import cliHistory from "@/components/History.vue";

export default {
  name: "home",
  components: {
    "cli-input": cliInput,
    "cli-history": cliHistory
  }
};
</script>

<style lang="scss" scoped>
.home {
  padding: 0px 100px;
  margin: 0;
  margin: 0 auto;
  text-align: center;
  color: #878787;
  display: flex;
  height: 100vh;
  flex-direction: column;
}

#cli- {
  &history {
    flex: 1 1 0;
  }

  &inputed {
    flex: 0 0 0;
  }
}
</style>
