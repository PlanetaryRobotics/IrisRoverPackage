<template>
  <div id="toolbar">
    <span> {{ B1 }} </span>
    <AtomicButton
      id="testButton1"
      substore="SAND"
      flavor="primary"
      text="Hello! I'm 1."
    />
    <span> {{ B2 }} </span>
    <AtomicButton
      id="testButton2"
      substore="SAND"
      flavor="primary"
      text="Hello! I'm 2."
    />
    <span> {{ B3 }} </span>
    <AtomicButton
      id="testButton3"
      substore="SAND"
      flavor="primary"
      text="Hello! I'm 3."
    />
    <span> {{ B4 }} </span>
    <AtomicButton
      id="testButton4"
      substore="SAND"
      flavor="primary"
      text="Hello! I'm 4."
    />
  </div>
</template>

<script>
import { mapState } from 'vuex';

export default {
    name: 'Toolbar',
    props: {
    },
    data(){
        return {
        };
    },
    computed: {
        ...mapState({
            B1: state => state.SAND.testButton1,
            B2: state => state.SAND.testButton2,
            B3: state => state.SAND.testButton3,
            B4: state => state.SAND.testButton4
        })
    }
};
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';
#toolbar {
  padding: .5rem 2rem;
}

</style>
