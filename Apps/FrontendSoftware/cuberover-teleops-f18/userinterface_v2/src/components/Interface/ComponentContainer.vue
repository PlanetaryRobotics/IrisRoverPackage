<template>
  <div class="componentContainer">
    <div class="componentContainer__header text__compHeader">
      <p>{{ header.name }}</p>
      <svg class="hamburger" viewBox="0 0 10 10" fill="none" xmlns="http://www.w3.org/2000/svg"><line x1="0.875" y1="1.125" x2="9.125" y2="1.125" stroke-linecap="round"/><line x1="0.875" y1="5.125" x2="9.125" y2="5.125" stroke-linecap="round"/><line x1="0.875" y1="9.125" x2="9.125" y2="9.125" stroke-linecap="round"/></svg>
    </div>
    <slot></slot>
  </div>
</template>

<script>
export default {
  props: ['header']
}
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

.componentContainer {
  display: grid;
  grid-template-rows: min-content 1fr;
  color: $color-grey-light;
  position: relative;
  max-height: 100vh;

  &__header {
    padding: .5rem 1.5rem;
    background-color: $color-background;
    border-bottom: 2px solid $color-near-black;
    display: flex;
    align-items: center;
  }
}

.hamburger {
  width: 1rem;
  height: 1rem;
  margin-left: 1rem;
  stroke: $color-grey-light;
  stroke-width: ".75";
}
</style>
