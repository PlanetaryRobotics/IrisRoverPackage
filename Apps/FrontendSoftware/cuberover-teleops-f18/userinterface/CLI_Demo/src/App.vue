<template>
  <div id="app">
    <router-view/>
  </div>
</template>

<style lang="scss">
*,
*::before,
*::after {
  margin: 0;
  padding: 0;
  box-sizing: inherit;
}

html {
  font-family: "Roboto Mono", monospace;
  background-color: #2d2d2d;
  box-sizing: border-box;
  font-size: 62.5%;
}

#app {
  font-size: 1.4rem;
}
</style>
