<template>
    <div class="AlphabetScroller">
      Scrolly
    </div> 
</template>

<script>

export default {
  name: "AlphabetScroller",
  components: {
   
  },
  data() {
    return {
    }
  },
  computed: {
  },
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.AlphabetScroller {
  background-color: $color-near-black;
  height: 100%;
}

</style>
