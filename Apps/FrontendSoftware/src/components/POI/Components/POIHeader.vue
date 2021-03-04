<template>
  <div class="POIHeaderContainer">
    <div class="POIHeader__importance">
      <div
        v-for="index in importanceSize"
        :key="index"
      >
        <svg
          width="2"
          height="16"
          viewBox="0 0 2 16"
          fill="none"
          xmlns="http://www.w3.org/2000/svg"
        >
          <line
            x1="1"
            y1="-4.37114e-08"
            x2="1"
            y2="16"
            :style="importanceColor"
            stroke-width="2"
          />
        </svg>
      </div>
    </div>

    <div
      class="POIHeader__title text__smallCaps--bold"
      :style="titleColor"
    >
      {{ getTitle() + POIData.number }}
    </div>
  </div>
</template>

<script>
import POICard from '@/data_classes/POICard.js';

export default {
    name: 'POIHeader',
    props: {
        POIData: Object,
    },
    computed: {
        importanceColor: function () {
            let category = this.POIData.category;
            let ans = POICard.CATEGORY_COLORS[category];
            return {
                stroke: ans,
            };
        },
        importanceSize: function () {
            let size = this.POIData.importanceLevel;
            return new Array(size);
        },
        titleColor: function () {
            let category = this.POIData.category;
            let ans = POICard.CATEGORY_COLORS[category];
            return {
                color: ans,
            };
        },
    },
    methods: {
        getTitle() {
            if (this.POIData.category === 'ATTRACTION') {
                return 'ATTR-';
            } else if (this.POIData.category === 'OBSTACLE') {
                return 'OBST-';
            } else {
                return 'SHDW-';
            }
        },
    },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";
@import "@/styles/_typography.scss";

.POIHeaderContainer {
  // padding-top: 1rem;
  padding-right: 1rem;
  display: flex;
  flex-direction: row;
}

.POIHeader {
  &__title {
    flex-grow: 1;
    padding-left: 1rem;
    display: flex;
    flex-direction: row;
  }

  &__importance {
    display: flex;
    flex-direction: row;
    width: 1rem;

    > div {
      padding-right: 2px;
    }
  }
}
</style>
