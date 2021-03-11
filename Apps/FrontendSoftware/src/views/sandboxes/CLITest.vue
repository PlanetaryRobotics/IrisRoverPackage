<template>
  <div class="home">
    <!-- Locations Relative to __static Directory, Found in /public -->
    <CLI />
  </div>
</template>

<script>
import CLI from '@/components/CLI/CLI.vue';

export default {
    name: 'Home',
    components: {
        CLI
    }
};
</script>

<style scoped>
.home {
  position: relative;
  top: 50px;
  left: 50px;
  width: calc(100vw - 100px);
  height: calc(100vh - 100px);
  background-color: rgb(66, 43, 38);
  /*top: 200px;
  left: 200px;
  width: 800px;
  height: 400px;*/
}/**/
</style>
