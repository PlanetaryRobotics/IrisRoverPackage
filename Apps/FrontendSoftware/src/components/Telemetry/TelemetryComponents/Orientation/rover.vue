<template slot="preview">
      <model-three :backgroundAlpha="0"
                   @on-load="onLoad"
                   src="static/models/json/scene.json">
      </model-three>
</template>

<script>
import ModelThree from '../Three/model-three.vue';

export default {
  name: 'rover',
  data() {
    return {
      loading: true,
    };
  },
  methods: {
    onLoad() {
      this.loading = false;
    },
  },
  components: {
    ModelThree,
  },
};
</script>

