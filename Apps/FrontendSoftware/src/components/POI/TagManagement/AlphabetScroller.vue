<template>
    <div class="AlphabetScroller">
      <div class="letter" v-for="(letter, index) in alphabetList" :key="index">
          {{letter}}
      </div>
    </div> 
</template>

<script>

export default {
  name: "AlphabetScroller",
  data() {
    return {
    }
  },
  computed: {
    alphabetList() {
      let set = new Set();
      Array.from(this.$store.getters.POIByTagList.keys())
            .forEach(elem => set.add(elem.getName()[0].toUpperCase()));
      
      return "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z".split(" ");
      //return [...set];
    }
  },
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.AlphabetScroller {
  padding-top: 16px;
  background-color: $color-near-black;
  height: 100%;
  display: flex;
  flex-direction: column;
  align-items: center;
}

.letter {
  font-weight: 600;
  font-size: (70vh/26);
  color: $color-grey-dark;
}

</style>
