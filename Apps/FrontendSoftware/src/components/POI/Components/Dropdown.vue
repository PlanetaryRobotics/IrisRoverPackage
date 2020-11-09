<template>
  <div class="dropdown-box" :tabindex="tabindex" @blur="open = false">
    <div class="selected" :class="{ open: open }" @click="open = !open">
      {{ selected }}
      <svg
        class="white-downwards-arrow"
        width="9"
        height="5"
        viewBox="0 0 9 5"
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
      >
        <path
          d="M1 1L3.74742 4.13991C4.14584 4.59524 4.85417 4.59524 5.25258 4.13991L8 1"
          stroke="#FCFCFC"
          stroke-linecap="round"
        />
      </svg>
    </div>
    <div class="items" :class="{ selectHide: !open }">
      <div
        v-for="(option, i) of options"
        :key="i"
        @click="
          selected = option;
          open = false;
          $emit('input', option);
        "
      >
        {{ option }}
      </div>
    </div>
  </div>
</template>

<script>
export default {
  props: {
    options: {
      type: Array,
      required: true,
    },
    default: {
      type: String,
      required: false,
      default: null,
    },
    tabindex: {
      type: Number,
      required: false,
      default: 0,
    },
  },
  data() {
    return {
      selected: this.default
        ? this.default
        : this.options.length > 0
        ? this.options[0]
        : null,
      open: false,
    };
  },
  mounted() {
    this.$emit("input", this.selected);
  },
};
</script>

<style scoped>
.dropdown-box {
  position: relative;
  text-align: left;
  outline: none;
  height: 2rem;
}

.dropdown-box .selected {
  background-color: #585858;
  border-radius: 0.4rem;
  color: #fff;
  padding: 0.4rem 0;
  padding-left: 1em;
  cursor: pointer;
  user-select: none;
}

.dropdown-box .selected.open {
  border-radius: 0.4rem 0.4rem 0px 0px;
}

/* styling of down/up facing arrow */
.white-downwards-arrow {
  margin-left: 3.6rem;
  margin-bottom: .1rem;
}

.dropdown-box .items {
  color: #fff;
  border-radius: 0px 0px 0.4rem 0.4rem;
  overflow: hidden;
  position: absolute;
  background-color: #424242;
  left: 0;
  right: 0;
  z-index: 1;
}

.dropdown-box .items div {
  color: #fff;
  padding: 0.4rem 0;
  padding-left: 1em;
  cursor: pointer;
  user-select: none;
}

.dropdown-box .items div:hover {
  background-color: #8a6dff;
}

.selectHide {
  display: none;
}
</style>