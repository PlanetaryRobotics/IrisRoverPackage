<template>
  <div id="toolbar">
    <span> {{B1}} </span>
    <AtomicButton substore="SAND" id="testButton1" flavor="primary" text="Hello! I'm 1." />
    <span> {{B2}} </span>
    <AtomicButton substore="SAND" id="testButton2" flavor="primary" text="Hello! I'm 2." />
    <span> {{B3}} </span>
    <AtomicButton substore="SAND" id="testButton3" flavor="primary" text="Hello! I'm 3." />
    <span> {{B4}} </span>
    <AtomicButton substore="SAND" id="testButton4" flavor="primary" text="Hello! I'm 4." />
  </div>
</template>

<script>
import { mapState } from 'vuex'

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
}
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';
#toolbar {
  padding: .5rem 2rem;
}

</style>
