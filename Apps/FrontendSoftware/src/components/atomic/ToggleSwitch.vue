<template>
  <div id="toggleSwitch">
    <div class="container">
      <label
        :id="id"
        class="switch"
      >
        <input
          type="checkbox" 
          :value="value"
          :checked="checked"
          @click="onClick"
        >
        <span class="slider round" />
      </label>

      <div class="slider-label">
        {{ label }}
      </div>
    </div>
  </div>
</template>

<script>
export default {
    name: 'ToggleSwitch',
    props: {
        id: {
            type: String,
            required: true
        },
        storeId: {
            type: String,
            required: true,
        },
        label: {
            required: true,
            type: String
        },
        checked: {
            type: Boolean,
            required: true,
        },
        value: {
            required: true,
            type: Object
        },
    },
    methods: {
        getPayload() {
            this.validateStoreParams(); //Validating here as cannot access $store in props
            return {id: this.id, store: this.$store.state[this.storeId]};
        },
        validateStoreParams() {
            // Check store exists
            if (this.$store.state[this.storeId] === undefined) {
                throw new Error('StoreId ' + this.storeId + ' is not found in main store.');
            }

            // Check id exists in store
            if (this.$store.state[this.storeId][this.id] === undefined) {
                throw new Error('Atomic id ' + this.id + ' does not exist in store ' + this.storeId);
            }
        },
        onClick() {
            let payload = this.getPayload();
            let initValue = this.$store.state[this.storeId][this.id].clicked;
            payload.value = !initValue;

            this.$store.commit('atomicClicked', payload);
        },
    }
};
</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';

.container {
  display: inline-block;
  position: relative;
  margin: 2rem;
}

.switch {
  position: absolute;
  display: inline-block;
  top: 0.5rem;
  width: 3rem;
  height: 1rem;
}

.switch input { 
  opacity: 0;
  width: 0;
  height: 0;
}

.slider-label {
  position: absolute;
  left: 4rem;
  color: #ffffff;
  font-size: 1.5rem;
  width: 15rem;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: $color-grey-dark;
  -webkit-transition: .4s;
  transition: .4s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 1rem;
  width: 1rem;
  left: 0rem;
  bottom: 0rem;
  background-color: $color-near-white;
  -webkit-transition: .4s;
  transition: .4s;
}

input:checked + .slider {
  background-color: $color-primary;
}

input:checked + .slider:before {
  -webkit-transform: translateX(2rem);
  -ms-transform: translateX(2rem);
  transform: translateX(2rem);
}

.slider.round {
  border-radius: 34px;
}

.slider.round:before {
  border-radius: 50%;
}

</style>
