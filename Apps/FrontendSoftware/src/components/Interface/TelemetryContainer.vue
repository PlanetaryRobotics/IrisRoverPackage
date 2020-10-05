<template>
  <div class="telemetryContainer">
    <div class="telemetryContainer__header text__compHeader" v-on:click="collapsed=!collapsed">
      <p>{{ header.name }}</p>
      <svg class="hamburger" viewBox="0 0 10 10" fill="none" xmlns="http://www.w3.org/2000/svg"><line x1="0.875" y1="1.125" x2="9.125" y2="1.125" stroke-linecap="round"/><line x1="0.875" y1="5.125" x2="9.125" y2="5.125" stroke-linecap="round"/><line x1="0.875" y1="9.125" x2="9.125" y2="9.125" stroke-linecap="round"/></svg>
    </div>
    <slot id="slot" v-if="!collapsed"></slot>
  </div>
</template>

<script>
export default {
  props: ['header'],
  data(){
    return {
      collapsed: false
    };
  }
}
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';

.telemetryContainer {
  display: flex;
  flex-direction: column;
  color: $color-grey-light;
  position: relative;
  background-color: $color-near-black;
  &__header {
    flex: 0 0 content;
    user-select: none;
    padding: 0.5rem 1.5rem;
    background-color: $color-background;
    border-bottom: 1px solid $color-grey;
    display: inline-flex;
    align-items: center;
    &>p{
      margin-bottom: 0.2rem;
    }
  }

}

.hamburger {
  width: 1rem;
  height: 1rem;
  margin-left: 1rem;
  margin-right: 1rem;
  stroke: $color-grey-light;
  stroke-width: .75;
}
</style>
