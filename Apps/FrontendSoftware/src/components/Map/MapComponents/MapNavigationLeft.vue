<template>
  <div id="mapNavigationLeft">
    <!-- BUFFER -->
    <div class="buffer" />

    <!-- BOTTOM OPTIONS-->
    <div class="bottomOptions">
      <svg
        class="icon"
        width="25"
        height="25"
        viewBox="0 0 25 25"
        fill="none"
        xmlns="http://www.w3.org/2000/svg"
        @click="togglePolarPlot"
      >
        <circle
          cx="12.5"
          cy="12.5"
          r="7"
          stroke="#E3E3E3"
        />
        <circle
          cx="12.5"
          cy="12.5"
          r="12"
          stroke="#E3E3E3"
        />
        <circle
          cx="12.5"
          cy="12.5"
          r="2.5"
          fill="#E3E3E3"
        />
      </svg>
    </div>
  </div>
</template>

<script>

export default {
    name: 'MapNavigationLeft',
    methods: {
        togglePolarPlot() {
            this.$store.commit('togglePolarPlotButton');
        }
    }
};

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

#mapNavigationLeft {
  z-index: 10;
  display: flex;
  flex-direction: column;
  justify-content: center;

  .topOptions {
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;
  }
  .buffer {
    flex-grow: 1;
  }
  .bottomOptions {
    display: flex;
    flex-direction: column;
    justify-content: center;
    align-items: center;

    >svg:last-child {
      margin-bottom: 2rem;
    }
  }
}

.icon {
  margin-top: 2rem;

  &:hover {
    cursor: pointer;
    stroke: $color-primary;
  }
}

</style>
