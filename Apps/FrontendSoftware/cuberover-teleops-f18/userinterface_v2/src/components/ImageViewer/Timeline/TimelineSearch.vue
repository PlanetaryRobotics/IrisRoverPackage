<!--
Search field that sorts images in timeline by tags.

Author: John Walker Moosbrugger, CMU
Created: 3/19/2019
Last Updated: 3/31/2019, Moosbrugger
-->

<template>
  <div id="timelineSearch">
    <div class="search">
      <div class="search__tags text__main" v-for="(tag, index) in searchedTags" :key="index">
        <p class="search__tags--item" @click="unsearchTag(tag)">
          {{ tag }}
        </p>
      </div>
      <input type="text" placeholder="Filter by tags" class="search__input text__main" v-on:keydown.enter="searchTag" v-model="searchText" @keydown.space.prevent>
    </div>
  </div>
</template>

<script>
import { mapState } from 'vuex';

export default {
  data: function() {
    return {
      searchText: "",
    }
  },
  computed: {
    ...mapState({
      searchedTags: state => state.IMG.searchedTags
    })
  },
  methods: {
    unsearchTag: function(tagName) {
      this.$store.commit('UNSEARCH_TAG', tagName)
    },
    searchTag: function() {
      if (this.searchText != "") {
        this.$store.commit('SEARCH_TAG', this.searchText)
        this.searchText = ""
      }
    }
  }
}
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

#timelineSearch {

}

.search {
  display: flex;
  width: min-content;
  background-color: $color-grey-dark;
  border-radius: 1rem;
  overflow: hidden;
  padding: .5rem .8rem;

  &__tags--item {
    padding: .3rem .8rem;
    border-radius: .5rem;
    background-color: $color-background;
    margin-right: .5rem;
    user-select: none;
    cursor: pointer;
    transition: .1s ease-in-out;

    &:hover {
      background-color: $color-primary;
    }
  }

  &__input {
    background-color: transparent;
    color: $color-near-white;
    border: none;
    width: 30rem;
    font-weight: 400;
    font-size: 1.35rem;
    padding: .3rem 0;

    &::placeholder {
      color: $color-grey;
    }

    &:focus {
      outline: none;
    }
  }
}
</style>
