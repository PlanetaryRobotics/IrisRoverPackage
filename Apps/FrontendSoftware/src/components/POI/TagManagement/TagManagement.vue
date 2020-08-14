<template>
    <div class="TagManagement scrollable">
        <div class="TagManagementHeader" v-for="(tag, index) in POITagList.keys()" :key="index">
          <TagManagementCard :tag="tag" :list="POITagList.get(tag)"/>
        </div>
    </div> 
</template>

<script>

import TagManagementCard from "@/components/POI/TagManagement/TagManagementCard.vue";

export default {
  name: "TagManagement",
  components: {
    TagManagementCard
  },
  computed: {
    POITagList() {
      return this.$store.getters.POIByTagList;
    }
  },
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';

.TagManagementHeader {
  display: flex;
  flex-direction: row;
  align-items: center;
}

</style>
