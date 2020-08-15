export default {
  mutations: {
    atomicClicked(state, {id, store, value}) {
        store[id]["clicked"] = value;
        console.log(store[id]["clicked"]);
        console.log(id + " clicked value changed to " + value);
    },

    atomicSliderChanged(state, {id, store, value}) {
      store[id]["value"] = value;
      console.log(JSON.stringify(store[id]["value"]));
      console.log(id + " value changed to " + JSON.stringify(value));
    },

    atomicHovered(state, {id, store, value}) {
      store[id]["hovered"] = value;
      console.log(id + " hover value changed to " + value);
    },
  }
};
