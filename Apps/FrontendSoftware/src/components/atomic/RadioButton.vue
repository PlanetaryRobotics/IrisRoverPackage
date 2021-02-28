<template>
  <label
    class="radioButton container"
    @mouseover="onMouseOver"
    @mouseleave="onMouseLeave"
  >
    {{ this.label }}

    <input
      v-if="initiallyChecked === null" 
      :id="id" 
      type="radio"
      name="radio"
      :value="value"
      disabled
    >

    <input
      v-if="initiallyChecked === false"
      :id="id" 
      type="radio"
      name="radio"
      :value="value"
      @change="onChange"
    >

    <input
      v-if="initiallyChecked === true"
      :id="id" 
      type="radio"
      name="radio"
      :value="value"
      checked
      @change="onChange"
    >

    <span class="checkmark" />
  </label>
</template>

<script>
import $ from 'jquery';

export default {
    name: 'RadioButton',
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
            required: true
        },
        value: {
            required: true
        },
        initiallyChecked: {
            required: true,
            validator: value => {
                // Null = disabled
                // False = not selected
                // True = selected
                return [null, true, false].indexOf(value) !== -1;
            }
        }
    },
    methods: {
        toggleSVGColor(id, isChecked) {
            let simpleSvg = $(`#${id}`).parent();

            if (isChecked) {
                simpleSvg.attr('class', 'svg-checked');
            } else {
                simpleSvg.attr('class', 'svg-unchecked');
            }
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
            let initValue = this.$store.state[this.storeId][this.id].clicked;
            payload.value = !initValue;

            this.updateDeselectedButtons();
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
        updateDeselectedButtons() {
            // Get encapsulating div of all radios
            let parent = $(`#${this.id}`).parent().parent();

            let curr = this;
            // Iterate over each child
            parent.children().each(function(i, elm){
                let elem = $( elm );
                // If class is a radioButton and id is NOT same as current, update the store's value for
                // that radioButton
                if (elem.hasClass('radioButton') && elem.find(':input').attr('id') !== curr.id) {
                    let deselectedId = elem.find(':input').attr('id');
                    let deselectedPayload = {id: deselectedId, 
                        store: curr.$store.state[curr.storeId],
                        value: false};
                    curr.$store.commit('atomicClicked', deselectedPayload);

                    if (elem.find('.svg-checked').length > 0){
                        let svgId = elem.find('.svg-checked svg').attr('id');
                        curr.toggleSVGColor(svgId, false);
                    }
                } 
            });
        }
    }
};
</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';

/* The container */
.container {
  color: white;
  display: block;
  font-size: 1.5rem;
  position: relative;
  padding-left: 2rem;
  margin: 2rem;
  cursor: pointer;
  -webkit-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
}

/* Hide the browser's default radio button */
.container input {
  position: absolute;
  opacity: 0;
  cursor: pointer;
}

/* Create a custom radio button */
.checkmark {
  position: absolute;
  top: 0.5rem;
  left: 0;
  height: 1rem;
  width: 1rem;
  border: 1px solid $color-grey;
  border-radius: 50%;
}

/* On mouse-over */
.container:hover input ~ .checkmark {
  border: 1px solid $color-primary;
}

/* When the radio button is checked, add a blue background */
.container input:checked ~ .checkmark {
  background-color: $color-primary;
  border: 1px solid $color-primary;
}

.container input:disabled ~ .checkmark {
  background-color: $color-grey-dark;
  border: 1px solid $color-grey-dark;
}

/* Create the indicator (the dot/circle - hidden when not checked) */
.checkmark:after {
  content: "";
  position: absolute;
  display: none;
}

/* Show the indicator (dot/circle) when checked */
.container input:checked ~ .checkmark:after {
  display: block;
}

/* Style the indicator (dot/circle) */
.container .checkmark:after {
  width: 1px;
  height: 1px;
  border-radius: 50%;
  background: $color-primary;
}

.svg-checked {
  stroke: $color-near-white !important;
}

.svg-unchecked {
  stroke: $color-grey-dark !important;
}

</style>
