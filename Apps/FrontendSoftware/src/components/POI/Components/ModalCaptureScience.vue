<template>
  <div class="modal-container">
    <ScienceImageIcon style="display: inline"></ScienceImageIcon> Capture
    Science Image
    <p id="desc">
      You are capturing ATTR-005 as a high-resolution science image. This action
      will take approximately <span class="thick">{{ time }}</span>.
    </p>
    <div class="fl-right">
      <button class="button modal-button">CANCEL</button>
      <button class="button modal-button confirm">CONFIRM</button>
    </div>
  </div>
</template>

<script>
import ScienceImageIcon from "@/assets/imgviewer/SVGcomponents/ScienceImageIcon.vue";

export default {
  name: "ModalCaptureScience",

  components: {
    ScienceImageIcon,
  },

  data() {
    return {
      time: "33 minutes",
    }
  },
}
</script>

<style scoped>
/* REM conversion: 1rem = 10px */
.modal-container {
  background: #2a2a2a;
  position: absolute;
  display: flex;
  width: 28rem; /* 120px/16px (per 1rem) = 7.5 rem; */
  flex-direction: column;
  border-radius: 0.4rem;
  padding: 1.6rem;

  font-size: 1rem;
  font-style: normal;
  font-weight: 300;
  text-align: left;
  z-index: 3;
}

#desc {
  padding-top: 2rem;
  padding-bottom: 2.8rem;
}

.thick{
  font-weight: 800;
}

/* Buttons */
.modal-button {
  font-size: 0.95rem;
  font-weight: 600;
  text-align: center;
  width: 6.5rem;
  float: right;
}

.confirm {
  background-color: #8a6dff;
  border-color: #8a6dff;
  margin-right: 0.8rem;
}
</style>