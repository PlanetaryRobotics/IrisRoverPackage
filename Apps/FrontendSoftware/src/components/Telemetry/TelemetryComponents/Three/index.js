import Vue from 'vue';

import ModelThree from './model-three.vue';

// alias
const ModelJson = Vue.extend(ModelThree, {
  name: 'model-json',
});

const components = [
  ModelJson,
];

/* eslint-disable no-shadow */
const install = (Vue) => {
  components.forEach((component) => {
    Vue.component(component.name, component);
  });
};

if (typeof window !== 'undefined' && window.Vue) {
  install(window.Vue);
}

export default {
  ModelJson,
};

export {
  ModelJson,
};
