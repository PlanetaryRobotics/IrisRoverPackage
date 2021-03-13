<template>
  <!-- <div class="modal-backdrop"> -->
  <div
    class="POIModalContainer"
    :style="positionModal"
  >
    <section
      class="modal-body"
      @click.stop="selectPOIChoice('Attraction')"
    >
      <slot name="body">
        <span class="dot-icon green" /> Attraction
      </slot>
    </section>

    <section
      class="modal-body"
      @click.stop="selectPOIChoice('Obstacle')"
    >
      <slot name="body">
        <span class="dot-icon red" /> Obstacle
      </slot>
    </section>

    <section
      class="modal-body"
      @click.stop="selectPOIChoice('Shadow')"
    >
      <slot name="body">
        <span class="dot-icon red" /> Shadow
      </slot>
    </section>
  </div>
  <!-- </div> -->
</template>

<script>
export default {
    name: 'POIModalChoiceList',

    props:{
        endCoordinates: Array,
    },

    computed:{
        positionModal(){
            let topPos = 'top: ' + ((this.endCoordinates[1]+67)/10) + 'rem;';
            let leftPost = 'margin-left: ' + ((this.endCoordinates[0]+1)/10) + 'rem';
            return topPos + leftPost;
        }
    },

    methods: {
        selectPOIChoice(val) {
            console.log('COL: ', val);
            this.$emit('POIChoiceSelected', val);
        },
    }
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
  position: fixed;
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