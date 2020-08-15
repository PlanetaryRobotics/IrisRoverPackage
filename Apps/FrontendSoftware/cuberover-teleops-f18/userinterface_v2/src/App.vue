<template>
  <div id="app" class="text__main">
    <router-view/>
  </div>
</template>

<style lang="scss">
@import '/styles/_functional.scss';
@font-face {
  font-family: 'Barlow';
  src: url('./styles/Barlow/Barlow-Regular.ttf')  format('truetype');
  font-weight: normal;
  font-style: normal;
}
@font-face {
  font-family: 'Barlow';
  src: url('./styles/Barlow/Barlow-SemiBold.ttf')  format('truetype');
  font-weight: bold;
  font-style: normal;
}
@font-face {
  font-family: 'Barlow';
  src: url('./styles/Barlow/Barlow-Italic.ttf')  format('truetype');
  font-weight: normal;
  font-style: italic;
}

body {
  background-color: $color-background;
  margin: 0;
  width: 100vw;
  height: 100vh;
}

*,
*::before,
*::after {
  margin: 0;
  padding: 0;
  box-sizing: inherit;
}

html {
  font-family: 'Barlow';
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: left;
  color: $color-near-black;

  box-sizing: border-box;
  font-size: 62.5%;
}

#app {
}
</style>
