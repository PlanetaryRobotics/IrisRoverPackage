<template>
  <label class="radioButton container" @mouseover = "onMouseOver" @mouseleave = "onMouseLeave">
    <input v-if="initiallyChecked === null" 
          type="radio" 
          name="radio"
          :id = "id"
          :value = "value"
          disabled>

    <input v-if="initiallyChecked === false"
          type="radio" 
          name="radio"
          :id = "id"
          :value = "value"
          >

    <input v-if="initiallyChecked === true"
          type="radio" 
          name="radio"
          :id = "id"
          :value = "value"
          checked
          >

     <simple-svg
      :filepath="getPath()"
      :width="'20px'"
      :height="'20px'"
      :id="getSVGid(id)"
      @ready="onSVGReady()"
      @click="onClick()"
      />
  </label>
</template>

<script>
import $ from 'jquery';
import VueSimpleSVG from 'vue-simple-svg';
import Vue from 'vue';

Vue.use(VueSimpleSVG);

export default {
  name: "RadioSVGButton",
  props: {
    id: {
      type: String,
      required: true
    },
    storeId: {
      type: String,
      required: true,
    },
    value: {
      required: true
    },
    svgName: {
      type: String,
      required: true,
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
    getSVGid(id) {
      return id + "SimpleSVG";
    },
    onSVGReady() {
      let comp = this;
      let id = this.getSVGid(this.id);

      // Set color depending on initialization
      this.toggleSVGColor(id, this.initiallyChecked);

      // Remove stroke on each child
      let svg = $(`#${id}`);
      svg.on("click", function() {
        comp.onClick();
      });

      // Remove stroke values from child components
      svg.children().each(function() {
        $(this).removeAttr('stroke');
      });
    },
    toggleSVGColor(id, isChecked) {
      // Using parent div of svg to manipulate checked/unchecked classes
      let simpleSvg = $(`#${id}`).parent();

      if (isChecked) {
        simpleSvg.attr("class", "svg-checked");
      } else {
        simpleSvg.attr("class", "svg-unchecked");
      }
    },
    getPath() {
      // Since webpack runs at build time, need to give it
      // hints for specifying parts of the path
      return require('@/assets/icons/'+this.svgName+'.svg');
    },
    getPayload() {
      this.validateStoreParams(); //Validating here as cannot access $store in props
      return {id: this.id, store: this.$store.state[this.storeId]};
    },
    validateStoreParams() {
      // Check store exists
      if (this.$store.state[this.storeId] === undefined) {
        throw new Error("StoreId " + this.storeId + " is not found in main store.");
      }

      // Check id exists in store
      if (this.$store.state[this.storeId][this.id] === undefined) {
        throw new Error("Atomic id " + this.id + " does not exist in store " + this.storeId);
      }
    },
    onClick() {
      let svgId = this.getSVGid(this.id);
      if ($(`#${svgId}`).parent().hasClass("svg-unchecked")){
        let payload = this.getPayload();
        let initValue = this.$store.state[this.storeId][this.id].clicked;
        payload.value = !initValue;

        // Toggle this SVG's stroke on
        this.toggleSVGColor(this.getSVGid(this.id), true);

        this.updateDeselectedButtons();
        this.$store.commit('atomicClicked', payload);
      }
    },
    onMouseOver() {
      let payload = this.getPayload();
      payload.value = true;

      this.$store.commit("atomicHovered", payload);
    },
    onMouseLeave() {
      let payload = this.getPayload();
      payload.value = false; 

      this.$store.commit("atomicHovered", payload);
    },
    updateDeselectedButtons() {
      // Get encapsulating div of all radios
      let parent = $(`#${this.id}`).parent().parent();

      let curr = this;

      // Iterate over each radio button
      parent.children().each(function(i, elm){
        let elem = $( elm );
        // If class is a radioButton and id is NOT same as current, update the store's value for
        // that radioButton
        if (elem.hasClass("radioButton") && elem.find(":input").attr("id") !== curr.id) {
          let deselectedId = elem.find(":input").attr("id")
          let deselectedPayload = {id: deselectedId, 
                                   store: curr.$store.state[curr.storeId],
                                   value: false};
          curr.$store.commit('atomicClicked', deselectedPayload);

          // If there as an svg that is checked, also toggle color
          if (elem.find(".svg-checked").length > 0){
            let svgId = elem.find(".svg-checked svg").attr("id")
            curr.toggleSVGColor(svgId, false);
          }
        } 
      });
    }
  }
}
</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';

/* The container */
.container {
  color: white;
  display: flex;
  justify-content: center;
  font-size: 1.5rem;
  position: relative;
  // margin: 2rem;
  cursor: pointer;
  -webkit-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
}

/* HIDE RADIO */
[type=radio] { 
  position: absolute;
  opacity: 0;
  width: 0;
  height: 0;
}

.svg-checked {
  stroke: $color-near-white;
}

.svg-unchecked {
  stroke: $color-grey-dark;
}

.svg-checked:hover {
  stroke: $color-primary;
}

.svg-unchecked:hover {
  stroke: $color-primary;
}

svg {
    float: left;
}
svg:nth-child(odd) {
    clear: left;
}

</style>