<template>
  <div
    id="slider"
    class="container"
  >
    <!-- HEADER -->
    <div class="functionalSlider__header">
      <p class="functionalSlider__header--title text__small headerText">
        {{ title }}
      </p>
      <p class="functionalSlider__header--value text__small headerText">
        +{{ formatValue() }}
      </p>
    </div>
    <!-- INPUT -->
    <input
      :id="id"
      v-model="childValue"
      type="range" 
      class="slider" 
      :min="minValue" 
      :max="maxValue"
      :step="step ? step : 0.2"
      @change="onChange" 
      @mouseover="onMouseOver"
      @mouseleave="onMouseLeave"
    >
  </div>
</template>

<script>

export default {
    name: 'Slider',
    props: {
        id: {
            type: String,
            required: true
        },
        storeId: {
            type: String,
            required: true,
        },
        title: {
            type: String,
            required: true
        },
        minValue: {
            type: Number,
            required: true
        },
        maxValue: {
            type: Number,
            required: true
        },
        step: {
            type: Number
        },
        value: {
            type: Number,
            required: true
        }
    },
    data() {
        return {
            // Have data take in prop's value to avoid
            // modification on parent's value
            childValue: this.value
        };
    },
    methods: {
        formatValue() {
            let val = parseFloat(this.childValue);
            return val.toFixed(2);
        },
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
        onChange() {
            let payload = this.getPayload();
            payload.value = this.childValue;

            this.$store.commit('atomicSliderChanged', payload);
        },
        onMouseOver() {
            let payload = this.getPayload();
            payload.value = true;

            this.$store.commit('atomicHovered', payload);
        },
        onMouseLeave() {
            let payload = this.getPayload();
            payload.value = false; 

            this.$store.commit('atomicHovered', payload);
        },
    }
};
</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

.container {
  margin: 2rem;
}

.functionalSlider {
  &__header {
    display: flex;
    justify-content: space-between;
  }
}

.headerText {
  color: white;
}

.slider {
  appearance: none;
  height: 4px;
  border-radius: 2px;
  background: $color-grey-dark;
  outline: none;
  width: 100%;
  transition: all .6s ease;

    &::-webkit-slider-thumb {
    -webkit-appearance: none;
    appearance: none;
    width: 1.3rem;
    height: 1.3rem;
    background: $color-primary;
    cursor: pointer;
    border-radius: 50%;
  }
}

</style>
