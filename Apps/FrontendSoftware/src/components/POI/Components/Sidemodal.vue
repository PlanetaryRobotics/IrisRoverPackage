<template>
  <div class = "sidemodal"
       :style="{ right: rightTranslation + 'px', 
                 top: topTranslation + 'px'}">
    <!-- HEADER -->
    <div class = "header text__main--bold">
      <div class = "header__row">
      {{header}}
      <svg class="removeButton" width="10" height="10" viewBox="0 0 10 10" fill="none" xmlns="http://www.w3.org/2000/svg">
          <path d="M8.23048 1.98065C8.31184 1.89929 8.31184 1.76738 8.23048 1.68602C8.14912 1.60466 8.01721 1.60466 7.93585 1.68602L4.95818 4.66369L1.98051 1.68603C1.89916 1.60467 1.76725 1.60467 1.68589 1.68603C1.60453 1.76738 1.60453 1.89929 1.68589 1.98065L4.66355 4.95832L1.68385 7.93802C1.60249 8.01938 1.60249 8.15129 1.68385 8.23265C1.76521 8.31401 1.89712 8.31401 1.97848 8.23265L4.95818 5.25295L7.93789 8.23266C8.01925 8.31401 8.15116 8.31401 8.23252 8.23266C8.31388 8.1513 8.31388 8.01939 8.23252 7.93803L5.25281 4.95832L8.23048 1.98065Z" fill="#FCFCFC"/>
      </svg>
      </div>
    </div>

    <!-- CONTENT IF-ELSE -->

    <!-- EDITABLE IMAGES -->
    <div class = "content" v-if='type==="IMAGES_EDITABLE"'>
      <div class="pill__image" v-for="(image, index) of data" :key="index" >
          {{image.timeForTagFormatting}}

          <svg @click="POICard.removeImage(image)" class="removeButton" width="10" height="10" viewBox="0 0 10 10" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M8.23048 1.98065C8.31184 1.89929 8.31184 1.76738 8.23048 1.68602C8.14912 1.60466 8.01721 1.60466 7.93585 1.68602L4.95818 4.66369L1.98051 1.68603C1.89916 1.60467 1.76725 1.60467 1.68589 1.68603C1.60453 1.76738 1.60453 1.89929 1.68589 1.98065L4.66355 4.95832L1.68385 7.93802C1.60249 8.01938 1.60249 8.15129 1.68385 8.23265C1.76521 8.31401 1.89712 8.31401 1.97848 8.23265L4.95818 5.25295L7.93789 8.23266C8.01925 8.31401 8.15116 8.31401 8.23252 8.23266C8.31388 8.1513 8.31388 8.01939 8.23252 7.93803L5.25281 4.95832L8.23048 1.98065Z" fill="#FCFCFC"/>
          </svg>
      </div>
    </div>

    <!-- HISTORY -->
    <div class = "content" v-if='type==="HISTORY"'>
      <div class="updateHistorySmall--line text__small" >
        <div>Created:</div>
        <div>{{data[0].user}}</div> 
        <div>{{data[0].time}}</div>
      </div>
      <div class="updateHistorySmall--line text__small" v-for="(history, index) of data.slice(1, data.length)" :key="index" >
        <div>Modified:</div>
        <div>{{history.user}}</div> 
        <div>{{history.time}}</div>
      </div>
    </div>

    <!-- END CONTENT -->
  </div>
</template>

<script>

import POICard from "@/data_classes/POICard.js";

export default {
  name: "Sidemodal",
  props: {
    POIListEl: HTMLDivElement,
    target: HTMLDivElement,
    header: String,
    POICard: POICard,
    type: {
      validator: function (value) {
        return ['IMAGES_EDITABLE', 'HISTORY'].indexOf(value) !== -1
      }
    },
    data: Array
  },
  data() {
    return {
      rightTranslation: null,
      topTranslation: null
    }
  },
  mounted() {
    debugger;
    this.setRightTranslation();
    this.setTopTranslation();

    // Add listener for scroll
    this.POIListEl.addEventListener("scroll", this.setTopTranslation);
  },
  destroyed(){
    // Remove listener when edit window is closed
    this.POIListEl.removeEventListener("scroll", this.setTopTranslation);
  },
  computed: {

  },
  methods: {
    setRightTranslation() {
      this.rightTranslation = this.POIListEl.getBoundingClientRect().width;
    },
    setTopTranslation() {
      this.topTranslation = this.target.getBoundingClientRect().top;
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';
@import '@/styles/_pill.scss';

$modalWidth: 30rem;

.sidemodal {
  position: absolute;
  width: $modalWidth;
  background-color: $color-near-black;
  z-index: 999;
  border-radius: 4px;
  box-shadow: 3px 3px 3px rgba(0, 0, 0, .28);
  padding: 1rem;
}

.header {
  margin-bottom: 1rem;
  &__row {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    align-items: center;
  }
}

.content {
  display: flex;
  flex-direction: row;
  flex-wrap: wrap;
}

.removeButton {
  &:hover {
    cursor: pointer;
    * {
      fill: $color-primary;
    } 
  }
}

.updateHistorySmall {
  padding: 0 2rem 2rem 2rem;

  > div {
    padding-right: 2px;
  }

  > div:nth-child(1) {
    padding-bottom: 1rem;
  }

  &--line {
    width: 100%;
    display: flex;
    flex-direction: row;
    justify-content: space-between;
  }
}

</style>
