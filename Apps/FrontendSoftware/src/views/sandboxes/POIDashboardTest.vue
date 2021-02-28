<template>
  <div class="home">
    <ComponentContainer :header="headers.POI">
      <POIDashboard />
    </ComponentContainer>
  </div>
</template>

<script>
import POIDashboard from '@/components/POI/POIDashboard.vue';
import ComponentContainer from '@/components/Interface/ComponentContainer.vue';

import { mapState } from 'vuex';

export default {
    name: 'Home',
    components: {
        POIDashboard,
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
    grid-template-rows: 1fr;
    background-color: $color-background;
    padding: .5rem;
  }

</style>
