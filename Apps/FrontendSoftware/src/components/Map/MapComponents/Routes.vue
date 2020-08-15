<template>
  <div id = "routes">
    <!-- HEADER --> 
    <div class="editTab">
      <div class="editTab__header" @click="toggleRoutes">
        <svg width="14" height="7" viewBox="0 0 8 4" fill="none" xmlns="http://www.w3.org/2000/svg" class="editTab__icon" :class="{ open : show.routes }">
        <path d="M1 0.5L3.29289 2.79289C3.68342 3.18342 4.31658 3.18342 4.70711 2.79289L7 0.5" stroke-linecap="round"/>
        </svg>
        <h2 class="text__main--bold editTab__title">Routes</h2>
      </div>

      <!-- ROUTE LIST --> 
      <div class="editTab__content" v-show = "show.routes">
        <div class="route">
          <!-- <div class="route__item" v-for="(route, index) in routeList" :key="index"> -->
          <div class="route__item" >
            <p class="route__item--name text__main"> Route1A </p>
            <div class="route__item--visibility">
              <svg width="19" height="14" viewBox="0 0 19 14" fill="none" xmlns="http://www.w3.org/2000/svg" class="routeIcon" :class="{routeIcon__selected: route.visible}" @click="toggleVisibility(route.name)">
                <circle cx="9.50019" cy="6.88459" r="2.76923"/>
                <path d="M9.5 1C4.92308 1 1 4.92308 1 6.88462C1 8.84615 4.92308 12.7692 9.5 12.7692C14.0769 12.7692 18 8.84615 18 6.88462C18 4.92308 14.0769 1 9.5 1Z"/>
              </svg>
            </div>
          </div>
        </div>
      </div>

    </div>
  </div>
</template>

<script>

import $ from 'jquery'

export default {
  name: "Routes",
  data() {
    return {
      show: {
        routes: false
      },
      route: {
        visible: true
      }
    }
  },
  methods: {
    toggleRoutes() {
      this.show.routes = !this.show.routes;
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

.editTab {
  border-bottom: 1px solid $color-near-black;
  padding: 1.5rem 2rem;

  &__header {
    display: flex;
    align-items: center;
    cursor: pointer;
    user-select: none;
  }

  &__icon {
    stroke: $color-grey-light;
    width: 14px;
    height: 14px;
    transform: rotate(-90deg);
    transition: .1s ease-in-out;
  }

  &__title {
    margin-left: 1rem;
  }
}

.route {
  padding-top: 2rem;

  &__item {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding-bottom: 1rem;

    &--visibility {
      display: flex;
      align-items: center;
    }
  }
}

.routeIcon {
  stroke: $color-grey-dark;
  transform: scale(.8);
  cursor: pointer;

  &__selected {
    stroke: $color-near-white;
  }
}

.open {
  transform: rotate(0deg);
}

#routes {
  background-color: $color-background;
}
</style>
