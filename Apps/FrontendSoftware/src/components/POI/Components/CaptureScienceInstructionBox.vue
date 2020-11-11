<template>
  <div id="top-instruction-box">{{ instruction1 }}</div>
</template>

<script>
export default {
  name: "CaptureScienceInstructionBox",

  props: {
    onParentClick: Boolean,
  },

  data() {
    return {
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
      let doneSpan = document.createElement("span");
      doneSpan.setAttribute("class", "done");
      doneSpan.innerHTML = "Done";
      //   doneSpan.onclick = ()=> this.completeCaptureSelection();
      doneSpan.onclick = this.completeCaptureSelection();
      document.getElementById("top-instruction-box").innerHTML =
        this.instruction2 + doneSpan.outerHTML;
    },

    completeCaptureSelection() {
      console.log("doneee");
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
  border-radius: 4rem;
  color: #fcfcfc;
  top: 5.1rem;
  margin-left: 25rem;

  font-size: 1.2rem;
  text-align: center;
}

.done {
  text-decoration: underline;
  z-index: 4;
}
</style>