<template>
  <div :id="id">
    <button
      v-if="enabled"
      :id="id"
      class="button"
      :class="flavor"
      :value="value"
      @mousedown="onMouseDown"
      @mouseup="onMouseUp"
      @mouseover="onMouseOver"
      @mouseleave="onMouseLeave"
    >
      {{ text }}
    </button>

    <button
      v-else
      :id="id"
      class="button"
      :class="flavor"
      :value="value"
      disabled
    >
      {{ text }}
    </button>
  </div>
</template>

<script>
export default {
    name: 'AtomicButton',
    props: {
        id: {
            type: String,
            required: true,
        },
        storeId: {
            type: String,
            required: true,
        },
        flavor: {
            type: String,
            required: true,
            validator: (value) => {
                return (
                    ['primary', 'nominal', 'danger', 'caution'].indexOf(value) !== -1
                );
            },
        },
        text: {
            required: true,
        },
        value: {
            required: true,
        },
        enabled: {
            required: true,
            validator: (value) => {
                // True = enabled
                // False = disabled
                return [true, false].indexOf(value) !== -1;
            },
        },
    },
    methods: {
        getPayload() {
            this.validateStoreParams(); //Validating here as cannot access $store in props
            return { id: this.id, store: this.$store.state[this.storeId] };
        },
        validateStoreParams() {
            // Check store exists
            if (this.$store.state[this.storeId] === undefined) {
                throw new Error(
                    'StoreId ' + this.storeId + ' is not found in main store.'
                );
            }

            // Check id exists in store
            if (this.$store.state[this.storeId][this.id] === undefined) {
                throw new Error(
                    'Atomic id ' + this.id + ' does not exist in store ' + this.storeId
                );
            }
        },
        onMouseDown() {
            let payload = this.getPayload();
            payload.value = true;

            this.$store.commit('atomicClicked', payload);
        },
        onMouseUp() {
            let payload = this.getPayload();
            payload.value = false;

            this.$store.commit('atomicClicked', payload);
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
    },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";

.button {
  border: none;
  color: white;
  padding: 0.5rem 1.5rem;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 1.5rem;
  border-radius: 4px;
  cursor: pointer;
  pointer-events: all;
}

.button:disabled {
  border: 1px solid $color-grey;
  color: $color-grey;
}

.button:disabled:hover {
  background-color: $color-grey-dark;
}

.primary {
  background-color: $color-primary;
  color: white;
  font-weight: 600;
}

.primary:hover {
  background-color: black;
  border: 1px solid $color-primary;
}

.nominal {
  border: 1px solid $color-nominal;
}

.nominal:hover {
  color: black;
  background-color: $color-nominal;
}

.danger {
  border: 1px solid $color-danger;
}

.danger:hover {
  color: black;
  background-color: $color-danger;
}

.caution {
  border: 1px solid $color-caution;
}

.caution:hover {
  color: black;
  background-color: $color-caution;
}
</style>
