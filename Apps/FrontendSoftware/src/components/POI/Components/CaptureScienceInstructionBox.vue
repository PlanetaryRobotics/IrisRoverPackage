<template>
  <div id="top-instruction-box"><span id="instruction">{{ instruction1 }}</span> <span class="done" v-if="instruction2Visible" v-on:click="completeCaptureSelection">Done</span></div>
</template>

<script>
export default {
  name: "CaptureScienceInstructionBox",

  props: {
    onParentClick: Boolean,
  },

  data() {
    return {
      instruction2Visible: false,
      instruction1: "Select POI in frame:",
      instruction2: "Capture science image with ATTR-005. ",
    };
  },

  watch: {
    onParentClick: function () {
      if (this.onParentClick == true) {
        this.onSelectPOI();
      }
    },
  },

  methods: {
    onSelectPOI() {
      document.getElementById("instruction").innerHTML = this.instruction2;
      this.instruction2Visible = true;
    },

    completeCaptureSelection() {
      this.$emit("captureSelectionSelected");
    },
  },
};
</script>

<style scoped>

#top-instruction-box {
  position:absolute;
  padding: 1.2rem 2.5rem;
  background-color: #585858;
  border-radius: 4rem;

  color: #fcfcfc;
  font-size: 1.5rem;
  text-align: center;
  z-index: 3;
}

.done {
  padding-left: 1.6rem;
  z-index: 4;
  font-weight: 400;

  text-decoration: underline;
}
</style>