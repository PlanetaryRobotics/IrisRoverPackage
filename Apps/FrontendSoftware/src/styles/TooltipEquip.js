'use strict';
/**
 * Provides a function to be called on mount in every component which uses tooltips 
 * (currently rendered using `tooltipster`) to apply proper tooltips.
 * 
 * Using Tooltips:
 * ----
 * - Give the target DOM element one of the `tooltip` classes and a `title` attribute containing the tooltip content.
 *      -- `tooltip` provides a normal tooltip.
 *      -- `tooltip_left` provides a tooltip which is always on the left side.
 *      -- `tooltip_right` provides a tooltip which is always on the left side.
 *      -- `tooltip_top` provides a tooltip which is always on the left side.
 *      -- `tooltip_bottom` provides a tooltip which is always on the left side.
 *      -- `tooltip_horizontal` provides a tooltip which is always on either the left or right side.
 *      -- `tooltip_vertical` provides a tooltip which is always on either the top or bottom side.
 * - Include the TooltipEquip function and call it in mounted on the DOM of the Vue component (`this.$el`).
 * - Import the `_functional.scss` Sass partial which contains the necessary styling.
 * 
 * Example:
 * ----
```
    <template>
        <div class="tooltip" title="Tooltip Content" />
    </template>

    <script>
        import TooltipEquip from '@/styles/TooltipEquip.js'

        export default {
            mounted(){
                TooltipEquip(this.$el);
            }
        }
    </script>

    <style lang="scss" scoped>
        @import '@/styles/_functional.scss';
    </style>
```
 * 
 * Author: Connor W. Colombo, CMU
 * Created: 08/14/2020
 * Last Update: 08/14/2020
 */

import $ from 'jquery';
import 'tooltipster';

// Activate All Tooltips:
const tooltipBaseProperties = {
    theme: ['tooltipster-borderless', 'tooltipster-borderless-customized'],
    animation: 'fade',
    animationDuration: [350, 0], // length of [opening, closing] animation
    delay: [200, 0], // delay before [opening, closing]
    trigger: 'custom',
    triggerOpen: {
        mouseenter: true,
        touchstart: true
    },
    triggerClose: {
        click: true,
        originClick: true,
        tap: true,
        touchleave: true,
        mouseleave: true,
        scroll: true
    }
};

// Applies the given tooltip `rule` object to DOM objects with the given `className` that are children of the given DOM object:
function applyTooltipRule(className, rule, parent=document){
    $(parent).find('.'+className).tooltipster(rule).removeClass(className).addClass(className+'--tooltip-applied');
}

// Applies the base tooltip ruleset along with the given `ruleExtension` object to DOM objects with the given `className` that are children of the given DOM object:
function applyTooltipExtension(className, ruleExtension, parent=document){
    applyTooltipRule(className, Object.assign(tooltipBaseProperties, ruleExtension), parent);
}

// Apply tooltips on DOM object mount to all children of the given DOM object:
function mountTooltips(parent=document){
    applyTooltipRule('tooltip', tooltipBaseProperties);
    applyTooltipExtension('tooltip_left', {
        side: 'left'
    }, parent);
    applyTooltipExtension('tooltip_right', {
        side: 'right'
    }, parent);
    applyTooltipExtension('tooltip_top', {
        side: 'top'
    }, parent);
    applyTooltipExtension('tooltip_bottom', {
        side: 'bottom'
    }, parent);
    applyTooltipExtension('tooltip_horizontal', {
        side: ['left', 'right']
    }, parent);
    applyTooltipExtension('tooltip_vertical', {
        side: ['top', 'bottom']
    }, parent);
}

export default mountTooltips;
  