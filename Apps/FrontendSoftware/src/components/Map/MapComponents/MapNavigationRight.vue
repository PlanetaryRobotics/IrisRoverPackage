<template>
  <div id="mapNavigationRight">
    <div class="functional">

      <div class="menu">
          <div class="menu__option">
            <svg class="icon" width="13" height="9" viewBox="0 0 13 9" fill="none" xmlns="http://www.w3.org/2000/svg">
            <circle cx="6.49991" cy="4.80777" r="1.74038" stroke="#E6E6E6" stroke-width="0.75"/>
            <path d="M6.5 1C3.53846 1 1 3.53846 1 4.80769C1 6.07692 3.53846 8.61539 6.5 8.61539C9.46154 8.61539 12 6.07692 12 4.80769C12 3.53846 9.46154 1 6.5 1Z" stroke="#E6E6E6" stroke-width="0.75"/>
            </svg>
          </div>
          <div class="menu__option">
            <svg class="icon" width="19" height="19" viewBox="0 0 19 19" fill="none" xmlns="http://www.w3.org/2000/svg">
            <rect x="10.248" y="2.91553" width="6.53936" height="6.53936" transform="rotate(45 10.248 2.91553)" fill="#595959"/>
            <path d="M10.5 17.5V14L16.7217 7.47363L10.248 1L3.77441 7.47363L9.32324 13.0225L6.54883 15.7969" stroke="#595959" stroke-width="0.5"/>
            <path d="M6.54883 13.9473V15.7969H8.39844" stroke="#595959" stroke-width="0.5"/>
            </svg>
          </div>
      </div>

    </div>
  </div>
</template>

<script>

export default {
  name: "MapNavigationRight",
  components: {
  },
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

.functional {
  display: grid;
  user-select: none;
  height: 100%;
  grid-template-rows: min-content 1fr min-content;
  background-color: $color-background;
  .menu {
    padding-top: 1rem;

    &__option {
      padding: 0.8rem 0.5rem;
      cursor: pointer;
    }
  }
}

.icon {
  stroke: $color-grey-dark;
  width: 3rem;
  transition: .1s ease-in-out;

  &__selected {
    stroke: $color-grey-light;
  }
}

</style>
