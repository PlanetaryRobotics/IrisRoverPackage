<template>
  <div class="home">
    <!-- TESTING ATOMIC COMPONENTS -->
    <div>
      <AtomicButton
        :id="'testButton'" 
        :flavor="'primary'" 
        :text="'Hi I am a button'" 
        :value="'testButton'" 
        :enabled="true"
        :store-id="'ATOMIC'"
      />

      <ToggleButton
        :id="'testToggleButton'"
        :flavor="'primary'"
        :text="'Hi I\'m a Toggle Button'" 
        :value="'testToggleButton'"
        :store-id="'ATOMIC'"
      />
      
      <ToggleSwitch
        :id="'testToggleSwitch'"
        :checked="false"
        :label="'Toggle Me'"
        :value="'testToggle'"
        :store-id="'ATOMIC'"
      />

      <RadioButton
        :id="'testDisabledRadio'"
        :initially-checked="null"
        :label="'I am a disabled radio button'"
        :value="'testDisabledRadio'" 
        :store-id="'ATOMIC'"
      />
      
      <RadioButton
        :id="'testDefaultRadio'"
        :initially-checked="false"
        :label="'I am a default radio button'"
        :value="'testDefaultRadio'" 
        :store-id="'ATOMIC'"
      />

      <RadioButton
        :id="'testSelectedRadio'"
        :initially-checked="true"
        :label="'I am a selected radio button'"
        :value="'testSelectedRadio'" 
        :store-id="'ATOMIC'"
      />
      
      <Slider
        :id="'testSlider'"
        :title="'testSlider'"
        :min-value="0"
        :value="5.2"
        :max-value="10.0"
        :step="0.2"
        :store-id="'ATOMIC'"
      />

      <CenterSlider
        :id="'testCenterSlider'"
        :title="'testCenterSlider'"
        :range="5"
        :value="2"
        :step="0.2"
        :store-id="'ATOMIC'"
      />

      <RangedSlider
        :id="'testRangedSlider'"
        :title="'testRangedSlider'"
        :min-value="0"
        :lower-input="1.2"
        :upper-input="9"
        :max-value="10"
        :step="0.2" 
        :store-id="'ATOMIC'"
      />

      <RadioSVGButton
        :id="'testSVG'"
        :initially-checked="false"
        :value="'testSelectedRadio'"
        :svg-name="'icon_cursor'" 
        :store-id="'ATOMIC'"
      />
    </div>
  </div>
</template>

<script>
import ToggleSwitch from '@/components/atomic/ToggleSwitch.vue';
import RadioButton from '@/components/atomic/RadioButton.vue';
import AtomicButton from '@/components/atomic/AtomicButton.vue';
import ToggleButton from '@/components/atomic/ToggleButton.vue';
import Slider from '@/components/atomic/Slider.vue';
import RangedSlider from '@/components/atomic/RangedSlider.vue';
import CenterSlider from '@/components/atomic/CenterSlider.vue';
import RadioSVGButton from '@/components/atomic/RadioSVGButton.vue';

import { mapState } from 'vuex';

export default {
    name: 'Home',
    components: {
        ToggleSwitch,
        RadioButton,
        AtomicButton,
        ToggleButton,
        Slider,
        RangedSlider,
        CenterSlider,
        RadioSVGButton
    },
    computed: {
        ...mapState({
            headers: state => state.headers,
        })
    },
};
</script>

<style scoped lang="scss">
@import '@/styles/_functional.scss';

  .home{
    height: 100vh;
    width: 100%;
    display: grid;
    grid-gap: .5rem;
    grid-template-columns: 50rem 1fr;
    grid-template-rows: 1fr;
    background-color: $color-near-black;
    padding: .5rem;
  }

</style>
