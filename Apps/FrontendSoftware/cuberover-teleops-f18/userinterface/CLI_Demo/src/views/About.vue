<template>
  <div class="about">
    <HelloWorld/>
  </div>
</template>

<script>
// @ is an alias to /src
import HelloWorld from "@/components/HelloWorld.vue";

export default {
  name: "about",
  components: {
    HelloWorld
  }
};
</script>

<style lang="scss" scoped>
.about {
  width: 600px;
  margin: 0 auto;
  text-align: center;
  color: #878787;
}
</style>
