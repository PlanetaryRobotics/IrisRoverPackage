<template>
  <div>
    <iframe
      id="fta-iframe"
      src="http://localhost:5000/display"
    />
  </div>
</template>

<script>

export default {
    name: 'FTA'
};
</script>

<style>
  #fta-iframe {
    position: absolute;
    width: 100%;
    height: 100%;
    border: none;
  }
</style>