<template>
  <!-- <div class="modal-backdrop"> -->
  <div class="POIModalContainer">
    <section v-on:click.stop="selectPOIChoice('Attraction')" class="modal-body">
      <slot name="body"> <span class="dot-icon green"></span> Attraction </slot>
    </section>

    <section v-on:click.stop="selectPOIChoice('Obstacle')" class="modal-body">
      <slot name="body"> <span class="dot-icon red"></span> Obstacle </slot>
    </section>

    <section v-on:click.stop="selectPOIChoice('Shadow')" class="modal-body">
      <!-- make an atomic modal
           popping up, moving, closing, common to all modals -->
      <slot name="body"> <span class="dot-icon red"></span> Shadow </slot>
    </section>
  </div>
  <!-- </div> -->
</template>

<script>
export default {
  name: "POIModalChoiceList",

  methods: {
    selectPOIChoice(val) {
      this.$emit("POIChoiceSelected", val);
    },
  },
};
</script>


<style>
/* .modal-backdrop {
    position: fixed;
    top: 0;
    bottom: 0;
    left: 0;
    right: 0;
    background-color: rgba(0, 0, 0, 0.3);
    display: flex;
    justify-content: center;
    align-items: center;
  } */

/* REM conversion: 1rem = 10px */
.POIModalContainer {
  background: #424242;
  overflow-x: auto;
  position: absolute;
  display: flex;
  width: 12rem; /* 120px/16px (per 1rem) = 7.5 rem; */
  flex-direction: column;
  border-radius: 0.4rem;

  font-size: 10px;
  font-style: normal;
  font-weight: 400;
  text-align: left;
  z-index: 3;
}

.modal-body {
  padding: 10px 12px;
}

.modal-body:hover {
  background-color: #8a6dff;
}

.dot-icon {
  width: 0.8rem;
  height: 0.8rem;
  top: 0.2rem;
  display: inline-block;
  margin-right: 0.8rem;
  margin-top: 0.1rem;
  border-radius: 50%;
}

.dot-icon.red {
  background-color: #ff2f46;
}

.dot-icon.green {
  background-color: #21df9a;
}
</style>