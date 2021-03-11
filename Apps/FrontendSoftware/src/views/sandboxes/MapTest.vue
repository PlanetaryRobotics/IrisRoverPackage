<template>
  <div class="home">
    <ComponentContainer :header="headers.MAP">
      <Map />
    </ComponentContainer>
  </div>
</template>

<script>
import Map from '@/components/Map/Map.vue';
import ComponentContainer from '@/components/Interface/ComponentContainer.vue';

import { mapState } from 'vuex';

export default {
    name: 'Home',
    components: {
        Map,
        ComponentContainer
    },
    computed: {
        ...mapState({
            headers: state => state.headers,
        })
    }
};
</script>

<style scoped lang="scss">
@import '@/styles/_functional.scss';

  .home{
    height: 100vh;
    width: 100%;
    display: grid;
    grid-gap: .5rem;
    // grid-template-columns: 50rem 1fr;
    grid-template-rows: 1fr;
    background-color: $color-background;
    padding: .5rem;
  }

  // .home > div {
  //   border: 1px solid white;
  //   background-color: rgba(233,171,88,.5);
  //   padding: 1em;
  //   color: #d9480f;
  // }


</style>
