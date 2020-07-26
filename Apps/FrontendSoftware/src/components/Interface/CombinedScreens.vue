<!--
Tabbed Grid Tool Allowing for Multiple Components ("screens") to be Combined and Viewed 
in the Window.

A series of DOM objects representing each of the "screens" to be combined is given in 
the default slot of this component. Each DOM object is then extracted from the default 
slot as a VNode and wrapped in a `packet` during #mount inside of the nodePackets array. 

Each packet contains an id as well as positioning and visibility information. Based on 
user interactions with the tabs, the positioning and visibility information in these 
packets is changed.

To render, the array of packets is appropriately sorted and filtered based on this 
information, then the VNodes are extracted from each remaining packet and put inside an 
instance of the VNodeRenderer functional component.

The positioning information is a simple ordering index maintained by the functions that modify them. 
In order to simplify sorting, the requirements for position values are that each packet has a unique
index and the indices must only range from 0 to Npackets-1. This must be observed carefully when 
reordering packets.

Author: Connor W. Colombo, CMU
Created: 07/12/2020
Last Update: 07/25/2020, Colombo
-->

<template>
  <div class="combined-screens">
    <div class="header"
      @drop='log("TODO: ADD DROP BEHAVIOUR TO HEADER")'
      @dragover.prevent
      @dragenter.prevent
    >
      <TabHeader
        v-for="packet in orderedNodePackets" :key="packet.id"
        :name="packet.name" 
        :active="Boolean(packet.visible)" :closeable="true" 
        :onClick="() => { packet.visible = true; log(`${packet.name} Clicked`); }"
        :onClose="() => { packet.visible = false; log(`${packet.name} Closed`); }"
        draggable="true"
        @dragstart.native='startDrag($event, packet)'
        @dragend.native='endDrag($event, packet)'
      />
    </div>
    <div class="grid-region">

      <div class="grid-hover-overlay" v-show="showHoverRegion">
        <div
          class="grid-hover-region--container"
          v-for="packet in orderedDisplayNodes" :key="packet.id"
        >
          <div class="grid-hover-region--indicator" />
          <div class="grid-hover-region">
            <div
              v-for="idx in 3"
              :key="idx"
              class="grid-hover-region--trigger"
              @dragover.prevent
              @dragenter.prevent="onDragEnter($event, idx)"
              @dragleave="onDragLeave($event)"
              @drop="onDropHandler($event, packet, idx);"
            />
          </div>
        </div>
      </div>
      
      <div class="main-grid">
        <div class="grid-node-container" v-for="packet in orderedDisplayNodes" :key="packet.id">
          <VNodeRenderer :node="packet.node" />
        </div>
      </div>

      <div class="egg-container" v-show="showEgg">
        <transition name="egg-bg-intro">
          <div v-if="showEgg" class="egg-space" />
        </transition>
        <transition name="egg-shell-intro">
          <div v-if="showEgg" class="egg-shell" v-html="eggShellSVG" />
        </transition>
        <transition name="egg-albumen-intro">
          <div v-if="showEgg" class="egg-albumen" v-html="eggAlbumenSVG" />
        </transition>
        <transition name="egg-yolk-intro">
          <div v-if="showEgg" class="egg-yolk" v-html="eggYolkSVG" />
        </transition>
      </div>
    </div>
  </div>
</template>

<script>
/* global __static */ // <- keep eslint from complaining about the __static directory
import path from 'path'
import fs from 'fs'

import TabHeader from '@/components/atomic/TabHeader.vue'

export default {
  name: 'CombinedScreens',
  components: {
    TabHeader,
    VNodeRenderer: {
        functional: true,
        render: (h,ctx) => ctx.props.node
    }
  },
  data(){
    return {
      nodePackets: [], // Array of packets containing the VNodes to be rendered as well as associated positioning metadata
      showHoverRegion: false,

      eggShellSVG: "", // Inline SVG HTML for Egg Shell. Shown when no tabs are selected.
      eggAlbumenSVG: "", // Inline SVG HTML for Egg Albumen. Shown when no tabs are selected.
      eggYolkSVG: "" // Inline SVG HTML for Egg Yolk. Shown when no tabs are selected.
    }
  },
  mounted() {
    console.log(this.$slots);
    console.log(this.$slots.default);

    this.eggShellSVG = fs.readFileSync(path.join(__static,'./egg/egg_shell.svg'), 'utf8');
    this.eggAlbumenSVG = fs.readFileSync(path.join(__static,'./egg/egg_albumen.svg'), 'utf8');
    this.eggYolkSVG = fs.readFileSync(path.join(__static,'./egg/egg_yolk.svg'), 'utf8');

    // Load Nodes from Default Slot and Create Node Packets:
    this.nodePackets = this.$slots.default.map( (x,idx) => ({ 
      id: idx, // Fixed unique identifier
      node: x, // Core VNode representing the contained DOM object
      name: x.data.attrs.TabName, // Name shown on the tab
      position: idx, // Mutible (indicates ordered position of tab)
      visible: 0 // Whether this packet should be rendered
    }));
  },
  computed: {
    // Array of Node Packets in the Appropriate Order (sorted by position property)
    orderedNodePackets(){
      return this.nodePackets.concat().sort( (a,b) => a.position - b.position );
    },
    // Ordered List of Nodes to be Displayed:
    orderedDisplayNodes(){
      return this.orderedNodePackets.filter( x => x.visible );
    },

    // Whether to Show the Egg (everything else is closed) - serves as a trigger for the transitions.
    showEgg(){
      return this.orderedDisplayNodes.length === 0;
    }
  },
  methods: {
    // When TabHeader is dragged, allow its data packet to be transferred through the move (into the drop zone)
    startDrag(event, packet){
      this.showHoverRegion = true;
      event.dataTransfer.dropEfect = 'move';
      event.dataTransfer.effectAllowed = 'move';
      event.dataTransfer.setData('packetId', packet.id);
    },
    // Cleanup when TabHeader drag ends:
    endDrag(){
      this.showHoverRegion = false;
    },

    // Determines the appropriate onDrop behaviours based on the given index of the drop trigger (1=left, 2=center, 3=right)
    onDropHandler(event, zonePacket, idx){
      switch(idx){
        case 1:
          console.log('left');
          this.onDropAddBefore(event, zonePacket.position);
        break;
        case 2:
          console.log('center');
          this.onDropSwap(event, zonePacket);
        break;
        case 3:
          console.log('right');
          this.onDropAddBefore(event, zonePacket.position+1);
        break;
      }
    },

    // Performs general maintenance procedures expected of all drop behaviours and Returns the packet and its id, extracted from the drop event dataTransfer.
    onDropHelper(event){
      const id = Number(event.dataTransfer.getData('packetId'));
      const packet = this.nodePackets.find( x => x.id === id );
      packet.visible = true;
      this.setHoverIndicatorHighlight(event, 0);
      return { packet, id };
    },

    // When TabHeader is dropped on a drop zone, receive its associated data packet and swap the packet with packet at the position of the drop zone (given by packet which is (currently) beneath the drop zone)
    onDropSwap(event, zonePacket){
      const { packet } = this.onDropHelper(event);
      if(zonePacket.id !== packet.id){
        zonePacket.visible = false;
      }
      this.setPacketPosition(packet, zonePacket.position);
    },
    // When TabHeader is dropped on a drop zone, receive its associated data packet and add the packet before the given position
    onDropAddBefore(event, rightPosition){
      const { packet } = this.onDropHelper(event);
      this.insertPacketBefore(packet, rightPosition);
    },

    // Gets the Appropriate Hover Indicator for the hover region trigger which generated the given event.
    getHoverRegion(event){
      return event.target.closest('.grid-hover-region--container').querySelector('.grid-hover-region--indicator');
    },

    // Correctly Positions the Hover Indicator when a Drag Event enters a Grid Region Trigger based on the given index of that trigger.
    onDragEnter(event, idx){
      const indicator = this.getHoverRegion(event);

      switch(idx){
        case 1:
          this.setObjectClass(indicator, 'left', 1);
          this.setObjectClass(indicator, 'center', 0);
          this.setObjectClass(indicator, 'right', 0);
        break;
        case 2:
          this.setObjectClass(indicator, 'left', 0);
          this.setObjectClass(indicator, 'center', 1);
          this.setObjectClass(indicator, 'right', 0);
        break;
        case 3:
          this.setObjectClass(indicator, 'left', 0);
          this.setObjectClass(indicator, 'center', 0);
          this.setObjectClass(indicator, 'right', 1);
        break;
      }

      this.setObjectClass(indicator, 'hovered', 1);
    },

    // Unhighlight indicator if drag is leaving the
    onDragLeave(event){
      // If not exiting to a sibling trigger, hide the original hover.
      if(!event.target.parentNode.isSameNode(event.relatedTarget.parentNode)){
        const indicator = this.getHoverRegion(event);
        this.setObjectClass(indicator, 'hovered', 0);
      }
    },

    // Highlights/Unhighlights the Hover Indicator for the Grid Region which generated the given event. (used for drag enter/leave/drop behaviour)
    // Given the event that generated the drag/drop event and the target hover state (0 or 1)
    setHoverIndicatorHighlight(event, state) {
      const indicator = this.getHoverRegion(event);
      this.setObjectClass(indicator, 'hovered', state);
    },

    // Sets the State of the Given Class of the Target of the Given Event (state=1 -> add, state=0 -> remove)
    setTargetClass(event, className, state){
      this.setObjectClass(event.target, className, state);
    },
    // Sets the State of the Given Class of the Given DOM Object (state=1 -> add, state=0 -> remove)
    setObjectClass(object, className, state){
      if(state){
        object.classList.add(className);
      } else{
        object.classList.remove(className);
      }
    },

    /**
     * Helper function to set the given packet's position to the given position while ensuring that:
     *  - No other packets occupy that position.
     *  - All packets continue to have continuous position numbers.
     *  - Every packet position in [0,Npackets) is occupied.
     *  - Only packet positions in [0,Npackets) are occupied.
     */
    setPacketPosition(targPacket, newPos){
      // Ensure new position satisfies traditional array boundaries:
      if(newPos >= this.nodePackets.length){
        newPos = this.nodePackets.length-1;
      } else if(newPos < 0){
        newPos = 0;
      }

      const original_packet_pos = targPacket.position;

      // Don't do anything if not moving packet somewhere new:
      if(newPos === original_packet_pos){ return; }

      // Shift around other packets:
      this.nodePackets.forEach( p => {
        // If not the target packet:
        if(p.id !== targPacket.id){
          // If moving packet up list, shift items between old and new position down to fill void:
          if(newPos > original_packet_pos && p.position > original_packet_pos && p.position <= newPos){
            p.position--;
          }
          // If moving packet down list, shift items between old and new position up to fill void:
          else if(newPos < original_packet_pos && p.position < original_packet_pos && p.position >= newPos){
            p.position++;
          }
        }
      });

      // Set packet pos to newPos:
      targPacket.position = newPos;
    },

    insertPacketBefore(targPacket, rightPos){
      // Ensure right position satisfies traditional array boundaries:
      if(rightPos > this.nodePackets.length){
        rightPos = this.nodePackets.length-1;
      } else if(rightPos < 0){
        rightPos = 0;
      }

      const original_packet_pos = targPacket.position;
      const leftPos = rightPos-1;

      // Don't do anything if not moving packet somewhere new:
      if(rightPos - 1 === original_packet_pos){ return; }

      this.nodePackets.forEach( p => {
        // If not the target packet:
        if(p.id !== targPacket.id){
          // Shift up if targPacket is shifting down the list and this packet comes after rightPos and before the hole opened up by the component being moved.
          if(rightPos < original_packet_pos && p.position >= rightPos && p.position < original_packet_pos) {
            p.position++;
          }
          // Shift down if targPacket is shifting up the list and this packet comes before leftPos and after the hole opened up by the component being moved.
          else if(original_packet_pos < leftPos && p.position <= leftPos && original_packet_pos < p.position){
            p.position--;
          }
        }
      });

      // Set packet pos to new position:
      // if coming down the list, take rightPos b/c the packet was there has been shifted out:
      if(rightPos < original_packet_pos) {
        targPacket.position = rightPos;
      }
      // if moving up the list, take leftPos b/c the packet was there has been shifted out:
      else if(original_packet_pos < leftPos){
        targPacket.position = leftPos;
      }
    },

    log(str){
      console.log(str);
    }
  }
}
</script>

<style scoped lang="scss">
@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

  $min-column-width: 10vw; // Minimum Column Width in the Cosmos Main Grid
  $min-row-height: 40vh; // Minimum Column Width in the Cosmos Main Grid

  $standard-padding: .5rem;

  .combined-screens {
    height: 100%;
    width: 100%;

    display: grid;
    grid-template-columns: 1fr;
    grid-template-rows: min-content minmax(2*$min-row-height, 1fr);
  }

  .header {
    display: flex;
    justify-content: flex-start;
    align-items: flex-end;

    padding-top: $standard-padding;
    
    border-bottom: $standard-padding solid $color-grey4;
  }

  .grid-region {
    display: grid;
    grid-template-columns: 1fr;
    grid-template-rows: repeat(auto-fit, minmax($min-row-height, 1fr));

    height: 100%;

    @mixin main-grid-structure-template(){
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax($min-column-width, 1fr));
      grid-template-rows: repeat(auto-fit, minmax($min-row-height, 1fr));

      grid-gap: $standard-padding;
      padding: $standard-padding;
    }

    & > .grid-hover-overlay {
      grid-column: 1;
      grid-row: 1 / span 2;
      
      @include main-grid-structure-template();
    }

    & > .main-grid {
      grid-column: 1;
      grid-row: 1 / span 2;

      @include main-grid-structure-template();

      background-color: $color-background;

      height: 100%;

      & > .grid-node-container {
        position: relative;
        overflow: hidden;
      }
    }
  }

  .grid-hover-region--container {
    background: transparent;
    height: 100%;
    width: 100%;

    display: grid;
    grid-template-rows: minmax(0, 1fr);
    grid-template-columns: minmax(0, 1fr);

    & > .grid-hover-region--indicator {
      z-index: 1000;

      height: 100%;
      width: 100%;

      grid-row: 1;
      grid-column: 1;

      &.hovered {
        background: $color-grey0;
        opacity: 0.2;
      }
      &:not(.hovered) {
        background: transparent;
      }

      &.left {
        width: 50%;
        justify-self: start;
      }
      &.right {
        width: 50%;
        justify-self: end;
      }
      &.center {
        width: 100%;
        justify-self: center;
      }
    }

    & > .grid-hover-region {
      z-index: 1001;

      height: 100%;
      width: 100%;
      
      grid-row: 1;
      grid-column: 1;

      display: grid;
      grid-template-rows: minmax(0, 1fr);
      grid-template-columns: repeat(3, 1fr);

      & > .grid-hover-region--trigger {
        background: transparent;
        grid-row: 1;
      }
    }
  }



  .egg {
    $egg-transition-start-delay: 30s;
    $egg-transition-full-duration: 10s;
    $egg-opacity: 1.0;

    &-bg-intro-enter-active {
      animation: egg-fade-in 0.15*$egg-transition-full-duration;
      animation-timing-function: linear;
    }
    &-bg-intro-enter-to {
      background: #000000;
    }
    &-shell-intro-enter-active {
      animation: egg-fade-in 0.8*$egg-transition-full-duration;
      animation-timing-function: linear;
      animation-fill-mode: backwards;
      animation-delay: $egg-transition-start-delay + 0.2*$egg-transition-full-duration;
    }
    &-albumen-intro-enter-active {
      animation: egg-fade-in 0.35*$egg-transition-full-duration;
      animation-timing-function: linear;
      animation-fill-mode: backwards;
      animation-delay: $egg-transition-start-delay + 0.4*$egg-transition-full-duration;
    }
    &-yolk-intro-enter-active {
      animation: egg-fade-in 0.4*$egg-transition-full-duration;
      animation-timing-function: linear;
      animation-fill-mode: backwards;
      animation-delay: $egg-transition-start-delay + 0.6*$egg-transition-full-duration;
    }
    @keyframes egg-fade-in-partial {
      0% {
        opacity: 0;
      }
      100% {
        opacity: $egg-opacity;
      }
    }

    @keyframes egg-fade-in {
      0% {
        opacity: 0;
      }
      100% {
        opacity: 1.0;
      }
    }

    &-container {
      grid-column: 1;
      grid-row: 1 / span 2;
      
      width: 100%;
      height: 100%;

      display: grid;
      grid-template-columns: minmax(0, 1fr);
      grid-template-rows: minmax(0, 1fr) minmax(0, 1fr);
      overflow: hidden;
    }

    &-space {
      z-index: 10;
      grid-column: 1;
      grid-row: 1 / span 2;

      width: 100%;
      height: 100%;

      background-color: #000000;
      opacity: $egg-opacity;
    }

    &-shell {
      z-index: 20;
      grid-column: 1;
      grid-row: 1 / span 2;
      opacity: $egg-opacity;

      align-self: end;
    }

    &-albumen {
      z-index: 30;
      grid-column: 1;
      grid-row: 1;
      opacity: 1.0;

      align-self: center;
    }

    &-yolk {
      z-index: 32;
      grid-column: 1;
      grid-row: 1;
      opacity: 1.0;

      align-self: center;
    }
  }
</style>
