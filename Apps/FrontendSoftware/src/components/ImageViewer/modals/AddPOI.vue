// Code inspo from AddTag.vue
// Last updated: October 15, 2020, by Gabbi LaBorwit

<template>
  <div class="addPOIPopUp modal">
    <h3 class="addPOIPopUp__header text__main--bold modal__header">ADD NEW POI</h3>
    <input type="text" class="input__text--reg text__main" placeholder="POI Name" @keydown.space.prevent="spacePressed" v-model="poiName">
    <p class="text__small addPOIPopUp__errorMessage">{{ errorMessage }}</p>
    <div class="addPOIPopUp__buttons">
      <button class="button button__subtle addPOIPopUp__buttons--input" @click="cancelClicked">CANCEL</button>
      <button class="button button__primary addPOIPopUp__buttons--input" @click="saveClicked">NEXT</button>
    </div>
  </div>
</template>

<script>
import { mapGetters } from 'vuex';

export default {
  props: ['addPOIName'],
  data: function() {
    return {
      errorMessage: "",
      poiName: ""
    }
  },

  computed: {
    ...mapGetters([
      'tagNames'
    ])
  },

  methods: {
    spacePressed: function() { // prevents spaces in tags and warns user
      this.errorMessage = "No spaces in tag names"
    },
    cancelClicked: function() { // cancels the tag creation process
      this.$store.commit('CANCEL_ADD_TAG')
    },
    saveClicked: function() { // creates new tag and assigns it to the orginal image
      if (this.poiName === "") {
        this.errorMessage = "Name cannot be empty"
      }
      else if (this.poiName.length > 30) {
        this.errorMessage = "Name is too long"
      }
      else if (this.tagNames.includes(this.poiName)) {
        this.errorMessage = "Name is already taken"
      }
      else {
        this.$store.commit('CREATE_TAG', {name: this.poiName, initialImage: this.addPOIName, tagNames: this.tagNames})
      }
    }
  }
}
</script>

<style lang="scss" scoped>
@import "@/styles/_functional.scss";

.addPOIPopUp {
  user-select: none;
  background-color: $color-primary;
  width: 45rem;

  &__header {
    padding-left: 1rem;
  }

  &__buttons {
    margin-top:1rem;
    padding-left: .2rem;

    &--input {
      margin-right: 1rem;
    }
  }

  &__errorMessage {
    padding-left: 1rem;
    margin-top: .5rem;
  }
}
</style>
