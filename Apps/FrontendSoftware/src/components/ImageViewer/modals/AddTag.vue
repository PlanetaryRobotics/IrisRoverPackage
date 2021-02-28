<template>
  <div class="addTag modal">
    <h3 class="addTag__header text__main--bold modal__header">
      Add New Tag
    </h3>
    <input
      v-model="tagName"
      type="text"
      class="input__text--reg text__main"
      placeholder="New Tag"
      @keydown.space.prevent="spacePressed"
    >
    <p class="text__small addTag__errorMessage">
      {{ errorMessage }}
    </p>
    <div class="addTag__buttons">
      <button
        class="button button__subtle addTag__buttons--input"
        @click="cancelClicked"
      >
        CANCEL
      </button>
      <button
        class="button button__primary addTag__buttons--input"
        @click="saveClicked"
      >
        SAVE
      </button>
    </div>
  </div>
</template>

<script>
import { mapGetters } from 'vuex';

export default {
    props: ['addTagName'],
    data: function() {
        return {
            errorMessage: '',
            tagName: ''
        };
    },

    computed: {
        ...mapGetters([
            'tagNames'
        ])
    },

    methods: {
        spacePressed: function() { // prevents spaces in tags and warns user
            this.errorMessage = 'No spaces in tag names';
        },
        cancelClicked: function() { // cancels the tag creation process
            this.$store.commit('CANCEL_ADD_TAG');
        },
        saveClicked: function() { // creates new tag and assigns it to the orginal image
            if (this.tagName === '') {
                this.errorMessage = 'Name cannot be empty';
            }
            else if (this.tagName.length > 30) {
                this.errorMessage = 'Name is too long';
            }
            else if (this.tagNames.includes(this.tagName)) {
                this.errorMessage = 'Name is already taken';
            }
            else {
                this.$store.commit('CREATE_TAG', {name: this.tagName, initialImage: this.addTagName, tagNames: this.tagNames});
            }
        }
    }
};
</script>

<style lang="scss" scoped>
@import "@/styles/_functional.scss";

.addTag {
  user-select: none;
  background-color: $color-primary;
  width: 30rem;

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
