<template>
  <div id="top-instruction-box">
    <span id="instruction">{{ instruction1 }}</span>
    <span v-if="instruction2Visible" class="obj-name">{{ name }}</span
    ><span v-if="instruction2Visible">.</span>
    <span
      class="done"
      v-if="instruction2Visible"
      v-on:click="completeCaptureSelection"
      >Done</span
    >
  </div>
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
      instruction2: "Capture science image with ",
      name: "ATTR-005",
    };
  },

  watch: {
    onParentClick: function () {
      if (this.onParentClick == false){
        console.log("uh oh");
        document.getElementById("instruction").innerHTML = this.instruction1;
        this.instruction2Visible = false;
      }
      else if (this.onParentClick == true) {
        console.log("trueee")
        this.onSelectPOI();
        this.$emit("instructionTwoActivated");
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
  position: absolute;
  padding: 1.2rem 2.5rem;
  background-color: #585858;
  border-radius: 2.8rem;
  top: 2rem;

  color: #fcfcfc;
  font-size: 1.5rem;
  font-weight: 600;
  text-align: center;
  z-index: 3;
}

.obj-name {
  color: #21DF9A;
}

.done {
  padding-left: 1.6rem;
  z-index: 4;
  font-weight: 400;

  text-decoration: underline;
}
</style>