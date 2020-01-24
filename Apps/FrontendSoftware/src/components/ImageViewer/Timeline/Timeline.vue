<!--
Container and layout for the timeline.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 10/6/2019, Colombo
-->

<template>
  <div id="timeline">
    <div class="timelineSearchCont">
      <TimelineSearch class="timelineSearch"/>
    </div>
    <div class="timelineImagesCont">
      <TimelineImages class="timelineImages"/>
    </div>
  </div>
</template>

<script>
import TimelineImages from "@/components/ImageViewer/Timeline/TimelineImages.vue"
import TimelineSearch from "@/components/ImageViewer/Timeline/TimelineSearch.vue"

export default {
  components: {
    TimelineImages,
    TimelineSearch
  }
}
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

#timeline {
  display: flex;
  flex-direction: column;
}

.timelineImages {
  position: relative;
  min-width: 0;
}

.timelineImagesCont {
  padding: 1rem 2rem;
}

.timelineSearchCont {
  padding: 1rem 2rem 1rem 2rem;
}
</style>
