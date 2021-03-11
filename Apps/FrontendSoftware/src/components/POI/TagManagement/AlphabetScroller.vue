<template>
  <div class="AlphabetScroller">
    <div
      v-for="(obj, index) in alphabetList"
      :id="getLetterId(obj)"
      :key="index"
      class="letter"
      @click="handleScroll(obj)"
    >
      {{ obj[0] }}
    </div>
  </div> 
</template>

<script>

export default {
    name: 'AlphabetScroller',
    data() {
        return {
            selectedId: null
        };
    },
    computed: {
        alphabetList() {
            let map = {};
            Array.from(this.$store.getters.POIByTagList.keys())
                .forEach(elem => {
                    let firstLetter = elem.getName()[0].toUpperCase();
                    // Only save first tag with a never before seen letter
                    if (!(firstLetter in map)) {
                        map[firstLetter] = elem.data.id;
                    }
                });
      
            map = Object.keys(map).map((key) => [key, map[key]]);
            return map;
        }
    },
    methods: {
    // Id for the sidebar letter
        getLetterId(obj) {
            return 'letter' + obj[1];
        },
        handleScroll(obj) {
            let elem;

            // Remove selected class if there was a prev. letter selected
            if (this.selectedId){
                elem = document.getElementById('letter'+this.selectedId);
                elem.classList.remove('selected');
            }

            // Scroll to target tag 
            this.selectedId = obj[1];
            document.getElementById(obj[1]).scrollIntoView({behavior: 'smooth', block: 'start', inline: 'nearest'});
      
            elem = document.getElementById('letter'+this.selectedId);
            elem.classList.add('selected');
        }
    }
};

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.AlphabetScroller {
  padding-top: 16px;
  height: 80%;
  display: flex;
  flex-direction: column;
  align-items: center;
}

.letter {
  font-weight: 600;
  font-size: (70vh/26);
  color: $color-grey-dark;
  flex-grow: 1;

  &:hover{
    color: white;
    cursor: pointer;
  }
}

.selected {
  color: white;
}

</style>
