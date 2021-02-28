<template>
  <div
    id="rangedSlider"
    class="container"
  >
    <!-- HEADER -->
    <div class="functionalSlider__header">
      <p class="functionalSlider__header--title text__small headerText">
        {{ title }}
      </p>
      <p class="functionalSlider__header--value text__small headerText">
        +{{ formatValue(childLowerInput) }}: +{{ formatValue(childUpperInput) }}
      </p>
    </div>
    <div
      :id="id"
      class="double-range"
      @mouseover="onMouseOver"
      @mouseleave="onMouseLeave"
    >
      <div class="double-range__bar" />
      <input
        type="range"
        class="double-range__range"
      >
      <!-- LOWER INPUT -->
      <input
        v-model="childLowerInput" 
        type="range" 
        class="double-range__thumb" 
        :min="minValue" 
        :max="maxValue"
        :step="step ? step : 0.2"
        @change="onChange"
      >
      <!-- UPPER INPUT -->       
      <input
        v-model="childUpperInput" 
        type="range" 
        class="double-range__thumb" 
        :min="minValue" 
        :max="maxValue"
        :step="step"
        @change="onChange"
      >
    </div>
  </div>
</template>

<script>

export default {
    name: 'RangedSlider',
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
            required: true,
            type: String
        },
        minValue: {
            type: Number,
            required: true
        },
        maxValue: {
            type: Number,
            required: true
        },
        lowerInput: {
            type: Number,
            required: true
        },
        upperInput: {
            type: Number,
            required: true
        },
        step: {
            type: Number
        },
    },
    data() {
        return {
            // Have data take in prop's value to avoid
            // modification on parent's value
            childLowerInput: this.lowerInput,
            childUpperInput: this.upperInput
        };
    },
    mounted() {

        // Adapted from: https://codepen.io/Striffly/pen/mwOXox
        window.addEventListener('load', () => {
            var trackSize = 0;

            var RangeSlider = function(el) {
                this.el = el;
                this.parent  = el.parentNode;
                this.range1 = elementByClassName(this.parent, 'double-range__thumb', 0);
                this.range2 = elementByClassName(this.parent, 'double-range__thumb', 1);
                this.bar = elementByClassName(this.parent, 'double-range__bar', 0);
                this.width = this.range1.offsetWidth;
                this.min = parseFloat(this.range1.min);
                this.max = parseFloat(this.range1.max);
                //this.displayElement = elementByClassName(this.parent, 'double-range__value', 0);

                this.update = function() {
                    var rangeValue1 = parseFloat(this.range1.value);
                    var rangeValue2 = parseFloat(this.range2.value);

                    if (rangeValue1 > rangeValue2) {
                        var tmp = rangeValue2;
                        rangeValue2 = rangeValue1;
                        rangeValue1 = tmp;
                    }

                    //this.displayElement.innerHTML = rangeValue1 + ' - ' + rangeValue2 + ' €';
                    this.bar.style.opacity = 1;
                    this.bar.style.width = ((((rangeValue2-rangeValue1)*(this.width-trackSize))/this.max)) + 'px';
                    this.bar.style.left = ((rangeValue1*(this.width-trackSize)/this.max) + (trackSize/2)) + 'px';

                // console.log("BAR WIDTH: " + this.bar.style.width);
                // console.log("BARD LEFT: " + this.bar.style.left);
                };
            };

            function toArray(data) {
                return [].slice.call(data);
            }

            function arrayByClassName(context, selector) {
                return toArray(context.getElementsByClassName(selector));
            }

            function elementByClassName(context, selector, offset) {
                return arrayByClassName(context, selector)[offset];
            }

            /*function isHover(e) {
                return (e.parentElement.querySelector(':hover') === e);
            }*/

            function detectIE() {
                var ua = window.navigator.userAgent;
                var response = false;

                var msie = ua.indexOf('MSIE ');
                if (msie > 0) {
                    response = true;
                }

                var trident = ua.indexOf('Trident/');
                if (trident > 0) {
                    response = true;
                }

                var edge = ua.indexOf('Edge/');
                if (edge > 0) {
                    response = true;
                }

                // other browser
                return response;
            }

            var ranges = arrayByClassName(document, 'double-range')
                .reduce(function (prev, htmlNodes) {
                    return prev.concat(arrayByClassName(htmlNodes, 'double-range__thumb'));
                }, []).filter(function (htmlNode) {
                    return htmlNode.type === 'range';
                });

            var isIE = detectIE();
      
            ranges.forEach(function (range) {
                range = new RangeSlider(range);

                if (isIE) {
                    range.range1.style.top = '8px';
                    range.range1.style.pointerEvents = 'auto';

                    range.range2.style.top = '18px';
                    range.range2.style.pointerEvents = 'auto';

                    range.el.addEventListener('change', function() { // not 'input' change for cross-compatibility
                        range.update();
                    });
                } else {
                    range.el.addEventListener('input', function() {
                        range.update();
                    });
                }


                range.update();
            });

        });
    },
    methods: {
        formatValue(val) {
            let valAsNum = parseFloat(val);
            return valAsNum.toFixed(2);
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
            payload.value = {lowerInput: this.childLowerInput, upperInput: this.childUpperInput};

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

// Parent range
.double-range {
  position: relative;
  width: 100%;
  height: 36px;
  text-align: center;
  overflow: hidden;
}

.double-range__range, .double-range__thumb {
  z-index: 1;
  pointer-events: none;
  position: absolute;
  overflow: hidden;
  left: 0;
  top: 15px;
  width: 100%;
  outline: none;
  margin: 0;
  padding: 0;
}

// Remove thumb for the highlight
.double-range__range ::-webkit-slider-thumb{
  -webkit-appearance: none;
  -moz-appearance: none;
  width: 0;
  height: 0;
}  

// Remove thumb for the highlight
.double-range__range::-webkit-slider-thumb{
  -webkit-appearance: none;
  -moz-appearance: none;
  width: 0;
  height: 0;
}  

// Thumbs
.double-range__thumb {
  -webkit-appearance: none;
  -moz-appearance: none;
  background: transparent;
  z-index: 3;
}

.double-range__thumb::-webkit-slider-thumb {
  pointer-events: auto;
  position: relative;
  z-index: 1;
  outline: 0;
  -webkit-appearance: none;
  -moz-appearance: none;
  background: transparent;
  z-index: 3;
  height: 1.3rem;
  width: 1.3rem;
  border-radius: 50%;
  background-color: $color-primary;
}

// Highlight in between user bounds
.double-range__bar {
  opacity: 0;
  z-index: 2;
  position: absolute;
  top: 21px;
  left: 0;
  width: 100%;
  height: 4px;
  /*height: calc(100% - 14px);*/
  background-color: rgba(165,109,255, 0.4);

  -webkit-transition: opacity .5s ease-in-out;
  -moz-transition: opacity .5s ease-in-out;
  -ms-transition: opacity .5s ease-in-out;
  -o-transition: opacity .5s ease-in-out;
  transition: opacity .5s ease-in-out;
}


// /* Adapted from: https://codepen.io/ChrisSargent/pen/meMMye */
// input[type=range] {
//    box-sizing: border-box;
//    appearance: none;
//    width: 100%;
//    margin: 0;
//    padding: 0 2px;
//    /* Add some L/R padding to ensure box shadow of handle is shown */
//    overflow: hidden;
//    border: 0;
//    border-radius: 1px;
//    outline: none;
//    background: linear-gradient($color-grey-dark, $color-grey-dark) no-repeat center;
//    /* Use a linear gradient to generate only the 2px height background */
//    background-size: 100% 4px;
//    pointer-events: none;

//    &:active,
//    &:focus {
//       outline: none;
//    }

//    &::-webkit-slider-thumb {
//       height: 1.3rem;
//       width: 1.3rem;
//       border-radius: 50%;
//       background-color: $color-primary;
//       position: relative;
//       margin: 5px 0;
//       /* Add some margin to ensure box shadow is shown */
//       cursor: pointer;
//       appearance: none;
//       pointer-events: all;
//       box-shadow: 0 1px 4px 0.5px rgba(0, 0, 0, 0.25);
//       &::before {
//             content: ' ';
//             //display: block;
//             position: absolute;
//             top: 13px;
//             left: 100%;
//             width: 200px;
//             height: 2px;
//       }
//    }
// }

// .multi-range {
//    position: relative;
//    height: 4px;

//    input[type=range] {
//       position: absolute;
      
//       &:nth-child(1) {
//          &::-webkit-slider-thumb::before {
//             background: $color-grey-dark;
//          }
//       }

//       &:nth-child(2) {
//          background: none;

//          &::-webkit-slider-thumb::before {
//              background: $color-primary;
//          }
//       }
//    }
// }

</style>
