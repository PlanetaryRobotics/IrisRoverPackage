<template>
  <div class="home">
    <!-- TESTING ATOMIC COMPONENTS -->
    <div>
      <AtomicButton :id="'testButton'" 
                    :flavor="'primary'" 
                    :text="'Hi I am a button'" 
                    :value="'testButton'" 
                    :enabled = "true"
                    :storeId = "'ATOMIC'"/>

      <ToggleButton :id="'testToggleButton'"
                    :flavor="'primary'"
                    :text="'Hi I\'m a Toggle Button'" 
                    :value="'testToggleButton'"
                    :storeId="'ATOMIC'"/>
      
      <ToggleSwitch :id="'testToggleSwitch'"
                    :checked="false"
                    :label="'Toggle Me'"
                    :value="'testToggle'"
                    :storeId="'ATOMIC'" />

      <RadioButton :id="'testDisabledRadio'"
                   :initiallyChecked="null"
                   :label="'I am a disabled radio button'"
                   :value="'testDisabledRadio'" 
                   :storeId="'ATOMIC'"/>
      
      <RadioButton :id="'testDefaultRadio'"
                   :initiallyChecked="false"
                   :label="'I am a default radio button'"
                   :value="'testDefaultRadio'" 
                   :storeId="'ATOMIC'"/>

      <RadioButton :id="'testSelectedRadio'"
                   :initiallyChecked="true"
                   :label="'I am a selected radio button'"
                   :value="'testSelectedRadio'" 
                   :storeId="'ATOMIC'"/>
      
      <Slider :id="'testSlider'"
              :title="'testSlider'"
              :minValue="0"
              :value="5.2"
              :maxValue="10.0"
              :step="0.2"
              :storeId="'ATOMIC'" />

      <CenterSlider :id="'testCenterSlider'"
                    :title="'testCenterSlider'"
                    :range="5"
                    :value="2"
                    :step="0.2"
                    :storeId="'ATOMIC'"  />

      <RangedSlider :id="'testRangedSlider'"
                    :title="'testRangedSlider'"
                    :minValue="0"
                    :lowerInput="1.2"
                    :upperInput="9"
                    :maxValue="10"
                    :step="0.2" 
                    :storeId="'ATOMIC'"/>

      <RadioSVGButton :id="'testSVG'"
                      :initiallyChecked="false"
                      :value="'testSelectedRadio'"
                      :svgName="'icon_cursor'" 
                      :storeId="'ATOMIC'"/>
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
  name: 'home',
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
}
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
