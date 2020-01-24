<template>
  <div class="home">
    <ComponentContainer class='CLI' v-bind:header="headers.CLI">
      <!-- Locations Relative to __static Directory, Found in /public -->
      <CLI commands="CLI_actions/sample-commands.csv" actionsDir="CLI_actions"/>
    </ComponentContainer>
    <ComponentContainer class='IMG' v-bind:header="headers.IMG">
      <ImageViewer />
    </ComponentContainer>
  </div>
</template>

<script>
import CLI from '@/components/CLI/CLI.vue'
import ImageViewer from '@/components/ImageViewer/ImageViewer.vue'
import ComponentContainer from '@/components/Interface/ComponentContainer.vue'
import { mapState } from 'vuex';

export default {
  name: 'home',
  components: {
    ImageViewer,
    ComponentContainer,
    CLI
  },
  computed: {
    ...mapState({
      headers: state => state.headers,
    })
  }
}
</script>

<style scoped lang="scss">
@import '@/styles/_functional.scss';

  .home{
    height: 100vh;
    width: 100%;
    display: grid;
    grid-gap: .5rem;
    grid-template-columns: 50rem 1fr;
    grid-template-rows: 1fr;
    background-color: $color-near-black;
    padding: .5rem;
  }

  .CLI {
    grid-column: 1 / 2;
    grid-row: 1 / 2;
    border-radius: 1rem;
    overflow: hidden;
  }

  .IMG {
    grid-column: 2 / 3;
    grid-row: 1 / 2;
    border-radius: 1rem;
    overflow: hidden;
  }
</style>
