<template>
  <div id="toolbar">
    <svg width="21" height="22" viewBox="0 0 21 22" fill="none" xmlns="http://www.w3.org/2000/svg" class="toolbar__icon">
      <path d="M5.7551 5.27801L8.20465 20.0044C8.3652 20.9696 9.67325 21.1537 10.0939 20.2703L12.2381 15.768C12.4163 15.3938 12.8068 15.1684 13.22 15.2011L18.1912 15.5954C19.1666 15.6727 19.6612 14.4479 18.9056 13.8263L7.37687 4.34168C6.66331 3.75464 5.60349 4.36653 5.7551 5.27801Z" stroke-linejoin="round"/>
    </svg>
  </div>
</template>

<script>
export default {
  
}
</script>

<style lang="scss" scoped>
@import '@/styles/_functional.scss';
#toolbar {
  padding: .5rem 2rem;
}

.toolbar__icon {
  stroke: $color-grey-light;
  transition: .1s ease-in-out;
  fill: $color-background;

  &:hover {
    fill: $color-grey-light;
  }
}
</style>
