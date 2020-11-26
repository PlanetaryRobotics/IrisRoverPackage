<template>
  <div class="modal-backdrop" v-on:click.self="closeModal">
    <div id="add-tag-container">
      <div class="modal-row">
        <div class="label">Tag Name*</div>
        <input type="text" placeholder="Tag Name" :value="[tagInput]" />
        <div class="sublabel">* A feature name is required.</div>
      </div>

      <div class="modal-row">
        <div class="label">Details</div>
        <textarea rows="6" placeholder="Fill out details here"></textarea>
      </div>

      <div id="btn-container">

          <button class="button modal-button" v-on:click="closeModal">
            CANCEL
          </button>

          <button class="button modal-button purple create">CREATE</button>
          
      </div>
    </div>
  </div>
</template>

<script>
export default {
  name: "AddTagModalWithinPOIModal",

  props: {
    tagInput: String,
  },

  data() {
    return {
    };
  },

  methods: {
      closeModal(){
        this.$emit("closeModal");
      }
  },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";
@import "@/styles/_functional.scss";

.modal-backdrop {
  position: fixed;
  top: 0;
  bottom: 0;
  left: 0;
  right: 0;
  background-color: rgba(0, 0, 0, 0.8);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 2;
}

/* REM conversion: 1rem = 10px */
#add-tag-container {
  background: #2a2a2a;
  overflow-x: auto;
  position: fixed;
  width: 24.2rem;
  border-radius: 0.4rem;
  padding: 1.6rem;

  font-size: 1rem;
  font-style: normal;
  font-weight: 400;
  text-align: left;
  z-index: 2;
}

.modal-row {
  padding: 0.8rem 0;
}

.modal-row:first-child {
  padding-top: 0;
}

.modal-row:last-child {
  padding-bottom: 0;
}

.label {
  margin-bottom: 0.8rem;
}

.sublabel {
  padding-top: 0.4rem;
  font-size: 0.7rem;
  font-weight: 400;
  text-align: left;
  color: #9f9f9f;
}

input,
textarea {
  background-color: $color-grey-dark;
  border: none;
  border-radius: 0.4rem;
  font-family: inherit;
  width: 100%;
  font-size: 10px;
  font-style: normal;
  font-weight: 400;
  padding: 0.4rem;
  color: #fcfcfc;

  &::-webkit-input-placeholder {
    color: #9f9f9f;
  }

  &::-moz-placeholder {
    color: #9f9f9f;
  }

  &::-ms-placeholder {
    color: #9f9f9f;
  }

  &::placeholder {
    color: #9f9f9f;
  }

  &:focus {
    outline: none !important;
  }
}

input {
  padding-left: 0.8rem;
}

textarea {
  overflow-y: scroll;
  resize: none;
  width: 100%;
  padding-left: 0.8rem;
}

#btn-container {
  float: right;
  margin-top:1.2rem;
}

// button styles
.modal-button {
  font-size: 0.95rem;
  font-weight: 600;
  text-align: center;
  width: 6.5rem;
}

.create {
  margin-left: 0.8rem;
}

.purple {
  background-color: $color-primary;
  border-color: $color-primary;
}

.purple:hover{
  background-color: #6148C5;
  border-color: #6148C5;
}
</style>