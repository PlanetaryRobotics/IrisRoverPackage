<template>
  <div>
    <div class="background" />
    <div class="deletemodal">
      <!-- HEADER -->
      <div class="header">
        <svg
          width="15"
          height="19"
          viewBox="0 0 15 19"
          fill="none"
          xmlns="http://www.w3.org/2000/svg"
          stroke="#ffffff"
        >
          <rect
            x="0.5"
            y="2.69006"
            width="13.2813"
            height="15.6615"
            rx="1.5"
            stroke-linecap="round"
          />
          <line
            x1="3.77295"
            y1="5.07031"
            x2="3.77295"
            y2="15.9714"
            stroke-linecap="round"
          />
          <line
            x1="7.04541"
            y1="5.07031"
            x2="7.04541"
            y2="15.9714"
            stroke-linecap="round"
          />
          <line
            x1="1.68994"
            y1="0.5"
            x2="12.591"
            y2="0.5"
            stroke-linecap="round"
          />
          <line
            x1="10.3184"
            y1="5.07031"
            x2="10.3184"
            y2="15.9714"
            stroke-linecap="round"
          />
        </svg>
        <div class="header__text text__main--bold">
          Delete Confirmation
        </div>
      </div>

      <!-- TEXT -->
      <div
        class="content"
        v-html="rawHTML"
      />

      <!-- BUTTONS -->
      <div class="buttonContainer">
        <button
          class="button__cancel"
          @click="executeCancel"
        >
          CANCEL
        </button>
        <button
          class="button__delete"
          @click="executeDelete"
        >
          DELETE
        </button>
      </div>
    </div>
  </div>
</template>

<script>

export default {
    name: 'Deletemodal',
    props: {
        rawHTML: String,
        deleteCallback: Function,
        cancelCallback: Function
    },
    methods: {
        executeCancel() {
            this.cancelCallback();
            this.closeModal();
        },
        executeDelete() {
            this.deleteCallback();
            this.closeModal();
        },
        closeModal() {
            this.$emit('closeModal', 'modalDelete');
        },
    }
};

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_typography.scss';
@import '@/styles/_pill.scss';

.background {
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: rgba(0, 0, 0, 0.7);
  z-index: 998;
}

.deletemodal {
  position: absolute;
  width: 36rem;
  height: 18rem;
  background-color: $color-near-black;
  margin: auto;
  position: absolute;
  top: 0; left: 0; bottom: 0; right: 0;
  z-index: 999;
  border-radius: 4px;
  box-shadow: 3px 3px 3px rgba(0, 0, 0, .28);
  padding: 2rem;
  display: flex;
  flex-direction: column;
  justify-content: center;
}

.header {
  display: flex;
  flex-direction: row;
  align-items: center;
  margin-bottom: 2rem;

  &__text {
    margin-left: 2rem;
  }
}

.content {
  line-height: 2rem;
  margin-bottom: 2rem;
}

.buttonContainer {
  display: flex;
  flex-direction: row;
  justify-content: flex-end;
}

.button {
  padding: 0.5rem 1.5rem;
  border-radius: 10px;

  &:focus {
    outline:none;
  }

  &__cancel{
    @extend .button;
    color: white;
    margin-right: 1rem;
    background-color: Transparent;

    &:hover {
      background-color: white;
      color: black;
      cursor: pointer;
    }
  }

  &__delete{
    @extend .button;
    border: 1px solid $color-danger;
    background-color: Transparent;
    color: $color-danger;

    &:hover {
      background-color: $color-danger;
      color: white;
      cursor: pointer;
    }
  }
}


</style>
