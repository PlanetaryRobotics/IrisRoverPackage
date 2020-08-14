<!--
Tabbed Grid Tool Allowing for Multiple Components ("screens") to be Combined and Viewed 
in the Window.

A series of DOM objects representing each of the "screens" to be combined is given in 
the default slot of this component. The `TabName` attribute in each component will 
specify the text shown in each respective tab.

Additionally, components can be supplied to the `modal-components` slot to be viewed in a 
modal container drawer docked to the side. The `ModalHeader` attribute in each such 
component will specify the text shown in the header (title) of the modal container when expanded.
The `ModalIcon` attribute in each will specify the CosmosIconPack icon to be used for the component 
drawer when closed.

Note: The order of components in slots matters. Tabs will be displayed (by default) left-to-right in 
the order given in the default slot, and modals will be displayed (by default) bottom-to-top in the 
order given.

Usage:
----
  <CombinedScreens>
    <SomeComponent TabName="First Component" />
    <div TabName="Second Component" class="can-be-bound-to-classes-in-parent-like-any-other-DOM-node"> 
      I'm just a normal div, but I could be any html tag with any attributes.
    </div>
    <AnotherComponent TabName="Etc..." />

    <template v-slot:modal-components>
      <OneComponent ModalHeader="A Component" ModalIcon="nameOfAnIconInCosmosIconPack" />
      <AnotherComponent ModalHeader="B Component" ModalIcon="nameOfAnotherIconInCosmosIconPack" />
      <Etc />
    </template>
  </CombinedScreens>

Operation:
----
Each DOM object is  extracted from the default slot as a VNode and wrapped in a `packet` 
during #mount inside of the nodePackets array.

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
Last Update: 08/07/2020, Colombo
-->

<template>
  <div class="combined-screens">
    <!-- DEFAULT HEADER (for when no tabs are selected): -->
    <div class="scrollable-header-container" ref="CSHeader" id="CSHeader" v-show="orderedDisplayNodes.length===0"> <!-- Note: can't use v-if for the ref to remain persistent -->
      <div class="header">
        <div class="header--tab-container">
          <TabHeader
            class="tab"
            v-for="tabPacket in orderedNodePackets" :key="tabPacket.id"
            :name="tabPacket.name"
            :active="Boolean(tabPacket.visible)"
            :closeable="tabPacket.visible"
            :onClick="() => { tabPacket.visible = true; }"
            :onClose="() => { tabPacket.visible = false; }"
            draggable="true"
            @dragstart.native='startDrag($event, tabPacket)'
            @dragend.native='endDrag($event, tabPacket)'
          />
        </div>
        <InfoBar />
      </div>
    </div>


    <!-- MODAL CONTAINERS: -->
    <ModalComponentContainer
      v-for="(node, idx) in $slots['modal-components']" :key="idx"
      :headerText="node.data.attrs.ModalHeader"
      :drawerIcon="node.data.attrs.ModalIcon"
    >
      <VNodeRenderer :node="node" />
    </ModalComponentContainer>


    <!-- MAIN GRID: -->
    <div class="grid-region">
      <div class="grid-row">
        <div 
          class="grid-column"
          v-for="(packet, columnIdx) in orderedDisplayNodes" :key="packet.id"
        >
          <!-- CONTAINED HEADER: -->
          <div class="scrollable-header-container" :id="`CSHeader${columnIdx}`">
            <div class="header">
              <div class="header--tab-container">
                <TabHeader
                  class="tab"
                  v-for="tabPacket in columnTabs(columnIdx)" :key="tabPacket.id"
                  :name="tabPacket.name" 
                  :active="Boolean(tabPacket.visible)"
                  :closeable="tabPacket.visible"
                  :showCloseOnlyOnHover="columnIdx===0 && orderedDisplayNodes.length===1"
                  :onClick="() => { packet.visible = false; tabPacket.visible = true; }"
                  :onClose="() => { tabPacket.visible = false; }"
                  draggable="true"
                  @dragstart.native='startDrag($event, tabPacket)'
                  @dragend.native='endDrag($event, tabPacket)'
                />
              </div>
              <InfoBar :syncedClock.sync="syncedClock" v-if="columnIdx===orderedDisplayNodes.length-1" />
            </div>
          </div>

          <!-- COLUMN HOVER REGIONS: -->
          <div class="grid-hover-region--container" v-show="showHoverRegion">
            <div class="grid-hover-region--indicator" />
            <div class="grid-hover-region">
              <div
                v-for="idx in 3"
                :key="idx"
                class="grid-hover-region--trigger"
                @dragover.prevent
                @dragenter.prevent="onDragEnter($event, idx)"
                @dragleave="onDragLeave($event)"
                @drop="onDropHandler($event, packet, idx, columnIdx)"
              />
            </div>
          </div>
          
          <!-- VNODE CONTENT: -->
          <div class="grid-node-container">
            <VNodeRenderer :node="packet.node" />
          </div>
        </div> <!--grid column-->
      </div> <!--grid row-->
    </div> <!--grid region-->

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

  </div> <!--combined screen-->
</template>

<script>
/* global __static */ // <- keep eslint from complaining about the __static directory
import path from 'path'
import fs from 'fs'

import TabHeader from '@/components/atomic/TabHeader.vue'
import InfoBar from '@/components/Interface/InfoBar.vue'
import ModalComponentContainer from '@/components/Interface/ModalComponentContainer.vue'

import Clock from '@/system/Clock.js'

export default {
  name: 'CombinedScreens',
  components: {
    TabHeader,
    InfoBar,
    ModalComponentContainer,
    VNodeRenderer: {
        functional: true,
        render: (h,ctx) => ctx.props.node
    }
  },
  data(){
    return {
      nodePackets: [], // Array of packets containing the VNodes to be rendered as well as associated positioning metadata
      showHoverRegion: false,

      // Clock synced between multiple InfoBar instances (one inside each header):
      syncedClock: new Clock(),

      eggShellSVG: "", // Inline SVG HTML for Egg Shell. Shown when no tabs are selected.
      eggAlbumenSVG: "", // Inline SVG HTML for Egg Albumen. Shown when no tabs are selected.
      eggYolkSVG: "" // Inline SVG HTML for Egg Yolk. Shown when no tabs are selected.
    }
  },
  mounted() {
    // Load Nodes from Default Slot and Create Node Packets:
    this.nodePackets = this.$slots.default.map( (x,idx) => ({ 
      id: idx, // Fixed unique identifier
      node: x, // Core VNode representing the contained DOM object
      name: x.data.attrs.TabName, // Name shown on the tab
      position: idx, // Mutible (indicates ordered position of tab)
      visible: idx===0 // Whether this packet should be rendered (make only first visible by default)
    }));

    this.eggShellSVG = fs.readFileSync(path.join(__static,'./egg/egg_shell.svg'), 'utf8');
    this.eggAlbumenSVG = fs.readFileSync(path.join(__static,'./egg/egg_albumen.svg'), 'utf8');
    this.eggYolkSVG = fs.readFileSync(path.join(__static,'./egg/egg_yolk.svg'), 'utf8');
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
    /** -- TAB FUNCTIONS --  **/

    // Returns the list of tabs which should be displayed above the column with the given index.
    // For the first column, it will be an array of all invisible tabs + the first visible tab; 
    // for the Nth column, it will just be the Nth visible tab (inside an array).
    columnTabs(colIdx){
      if(colIdx === 0){
        return this.orderedNodePackets.filter( p => !p.visible || p.id===this.orderedDisplayNodes[0].id );
      } else {
        return [this.orderedDisplayNodes[colIdx]];
      }
    },

    /** -- TAB DRAG FUNCTIONS --  **/

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

    /**
     * Determines the appropriate onDrop behaviours based on the given index of the drop trigger (1=left, 2=center, 3=right).
     * 
     * If the dropped columnIdx is 0, special behaviour will take place to ensure that all invisible packets come before the 
     * second visible packet.
     * columnIdx
     */
    onDropHandler(event, zonePacket, idx, columnIdx){
      // Special case for first column in a row:
      if(columnIdx === 0){
        // Get the highest position of all invisible packets:
        let maxInvisiblePos = this.orderedNodePackets
                                    .filter( p => !p.visible )
                                    .reduce( (maxPos, p) => p.position > maxPos ? p.position : maxPos, -1 );
        switch(idx){
          case 1:
            // Correctly position new packet:
            this.onDropAddBefore(event, zonePacket.position);
            // The reposition old packet (in drop zone) to after the invisibles:
            //this.insertPacketBefore(zonePacket, maxInvisiblePos+1); // <- this breaks ordering when you try to move the last window before the penultimate window (can't create an edge case where it appears necessary but it seemed necessary at one point...)
          break;
          case 2:
            this.onDropReplace(event, zonePacket);
          break;
          case 3:
            // If new packet is supposed to come after this one, move it so that it comes after all invisible packets too.
            this.onDropAddBefore(event, Math.max(maxInvisiblePos, zonePacket.position)+1);
          break;
        }
      } 
      // Standard Behaviour:
      else{
        switch(idx){
          case 1:
            this.onDropAddBefore(event, zonePacket.position);
          break;
          case 2:
            this.onDropReplace(event, zonePacket);
          break;
          case 3:
            this.onDropAddBefore(event, zonePacket.position+1);
          break;
        }
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

    // When TabHeader is dropped on a drop zone, receive its associated data packet and replaces packet the position of the drop zone with the packet being dragged (referenced by id in drag event's dataTransfer).
    onDropReplace(event, zonePacket){
      const { packet } = this.onDropHelper(event);
      this.replacePacketWith(zonePacket, packet);
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
     * Effectively replaces `packetOut` with `packetIn` by inserting 
     * `packetIn` in the position of `packetOut` and making `packetOut`
     * invisible while making `packetIn` visible;
     */
    replacePacketWith(packetOut, packetIn){
      if(packetIn.id !== packetOut.id){ // Don't bother doing anything if packets are the same.
        this.setPacketPosition(packetIn, packetOut.position);
        packetIn.visible = true;
        packetOut.visible = false;
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

    /**
     * Helper function to set the given packet's position to the given 
     * position while preserving the intended order by ensuring that the
     * packet comes right before the given index.
     * Subtly but significantly distinct from #setPacketPosition.
     *  - No other packets occupy that position.
     *  - All packets continue to have continuous position numbers.
     *  - Every packet position in [0,Npackets) is occupied.
     *  - Only packet positions in [0,Npackets) are occupied.
     */
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
    }
  }
}
</script>

<style scoped lang="scss">
@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_dimensions.scss';

  $min-column-width: 10vw; // Minimum Column Width in the Cosmos Main Grid
  $min-row-height: 40vh; // Minimum Column Width in the Cosmos Main Grid

  .combined-screens {
    height: 100%;
    width: 100%;

    display: grid;
    grid-template-columns: 1fr;
    grid-template-rows: min-content minmax(2*$min-row-height, 1fr);
  }


  .scrollable-header-container {
    // Custom Horizontal Scroll Behaviour (make thumb appear over usual divider):
    // (looks like normal border when unscrollable)
    $bg-grey: $color-background;
    $track-grey: $color-grey4;

    background: $track-grey; // use same color bg as bar to cover up small gap between bar and content
    @include scrolling-aesthetics();
    overflow-x: scroll; // Force scrollbar to be permanently visible (thumb automatically hidden when unscrollable)

    &::-webkit-scrollbar {
      height: $std-padding;
    }

    &::-webkit-scrollbar-track {
      background: $track-grey;
    }

    & > .header {
      // Fix background to what it should be behind tabs (to cover up track-grey bg in outer scrollable region)
      min-width: max-content;
      background: $bg-grey;
      display: flex;
      justify-content: space-between;
      align-items: stretch;

      padding-top: $std-padding;

      & > .header--tab-container {
        min-width: max-content;
        display: flex;
        justify-content: flex-start;
        align-items: flex-end;

        & > .tab {
          flex-shrink: 0;
          margin-right: $std-thin-margin;

          &:active {
            cursor: grabbing;
          }
        }
      }
    }
  }


  .grid-region {
    grid-column: 1;
    grid-row: 2 / span 2;
      
    display: grid;
    grid-template-columns: 1fr;
    grid-template-rows: repeat(auto-fit, minmax($min-row-height, 1fr));

    height: 100%;

    @mixin grid-row-structure-template(){
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax($min-column-width, 1fr));
      grid-template-rows: repeat(auto-fit, minmax($min-row-height, 1fr));

      grid-gap: $std-padding;
    }

    & > .grid-row {
      grid-column: 1;
      grid-row: 1 / span 2;

      @include grid-row-structure-template();

      background-color: $color-background;

      height: 100%;

      & > .grid-column {
        height: 100%;
        width: 100%;

        display: grid;
        grid-template-columns: minmax($min-column-width, 1fr);
        grid-template-rows: min-content minmax($min-row-height, 1fr);

        & > .scrollable-header-container { // Also add a second border to the containerized headerz
          border-bottom: {
            style: solid;
            color: $color-background;
            width: $std-spacing;
          };
        }
      }
    }
  }

  .grid-node-container {
    grid-column: 1;
    grid-row: 2;

    position: relative;
    overflow: hidden;
  }

  .grid-hover-region--container {
    grid-column: 1;
    grid-row: 2;

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
      grid-row: 2 / span 2;
      
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
