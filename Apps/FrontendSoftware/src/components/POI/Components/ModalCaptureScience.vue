<template>
  <div class="modal-container">
    <div class="flex-container flex-vertical-center">
      <ScienceImageIcon /> <span class="title thick">Capture
        Science Image</span>
    </div>
    <p id="desc">
      You are capturing ATTR-005 as a high-resolution science image. This action
      will take approximately <span class="thick">{{ time }}</span>.
    </p>
    <div class="flex-container button-container">
      <button
        class="button modal-button"
        @click="closeModal"
      >
        CANCEL
      </button>
      <button
        class="button modal-button confirm"
        @click="confirmImage"
      >
        CONFIRM
      </button>
    </div>
  </div>
</template>

<script scoped>
import ScienceImageIcon from '@/assets/imgviewer/SVGcomponents/ScienceImageIcon.vue';

export default {
    name: 'ModalCaptureScience',

    components: {
        ScienceImageIcon,
    },

    data() {
        return {
            time: '33 minutes',
        };
    },

    methods: {
        closeModal() {
            this.$emit('closeCapSciModal');
        },
        confirmImage(){
            this.$emit('confirmCapSciImage');
        }
    },
};
</script>

<style scoped>
/* REM conversion: 1rem = 10px */
.modal-container {
  background: #2a2a2a;
  position: fixed;
  align-self: center;
  display: flex;
  width: 28rem; /* 120px/16px (per 1rem) = 7.5 rem; */
  flex-direction: column;
  border-radius: 0.4rem;
  padding: 1.6rem;

  font-size: 1rem;
  font-style: normal;
  font-weight: 400;
  text-align: left;
  z-index: 3;
}

#desc {
  padding-top: 2rem;
  padding-bottom: 2.8rem;
}

.flex-container{
  display: flex;
}

.flex-vertical-center{
  align-items: center;
}

.title{
  margin-left: .4rem;
}

.thick {
  font-weight: 700;
}

/* Buttons */
.button-container{
  justify-content: flex-end;
}

.modal-button {
  font-size: 0.95rem;
  font-weight: 600;
  text-align: center;
  width: 6.5rem;
  /* float: right; */
}

.confirm {
  background-color: #8a6dff;
  border-color: #8a6dff;
  margin-left: 0.8rem;
}

.confirm:hover{
  background-color: #6148C5;
  border-color: #6148C5;
}
</style>