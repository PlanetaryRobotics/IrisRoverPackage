<!--
Contains any component within a modal window which is docked to either side of the screen 
and can be moved around vertically or hidden (minimized) inside a drawer docked to the side. 
Component must be given through the primary slot.

Note: Only designed to work as a child of `ComponentScreens.vue` and mainly exists as a 
way of cleaning up and containerizing the code for that component. Accordingly, this 
component requires access to the `CSHeader` $ref in the $parent component `ComponentScreens`.

Usage:
----
<ModalComponentContainer headerText="Name Shown in Title Bar" drawerIcon='commandLine'>
    <SomeComponent />
</ModalComponentContainer>

Author: Connor W. Colombo, CMU
Created: 07/31/2020
Last Update: 08/14/2020, Colombo
-->

<template>
  <div
    class="modal-container"
    :class="{ 
      'maximized': modalMaximized,
      'left': modalContainerIconData.side === 'left', 
      'right': modalContainerIconData.side === 'right' 
    }"
    :style="{ top: modalMaximized ? '' : modalContainerIconData.top }"
  >
    <div
      v-show="!showModalContent"
      class="modal-container--icon tooltip_horizontal"
      :class="{
        'left': modalContainerIconData.side === 'left', 
        'right': modalContainerIconData.side === 'right' 
      }"
      :title="`Click to open ${headerText}. Drag to Move.`"
      draggable="true"
      @click="openModal"
      @dragstart="modalDragStart($event)"
      @drag.prevent="modalOnDrag($event)"
    >
      <div
        class="modal-container--icon--box svg-clickless-container"
        v-html="CosmosIconPack(drawerIcon)"
      />
    </div>
    <div 
      v-show="showModalContent"
      class="modal-content-container"
      :class="{ 
        'maximized': modalMaximized,
        'left': modalContainerIconData.side === 'left', 
        'right': modalContainerIconData.side === 'right'
      }"
    >
      <div
        class="modal-content-container--header"
        draggable="true"
        @dblclick="modalMaximized = !modalMaximized"
        @dragstart="modalDragStart($event)"
        @drag.prevent="modalOnDrag($event)"
      >
        <div class="modal-content-container--header--title">
          {{ headerText }}
        </div>
        <div class="modal-content-container--header--buttons">
          <div
            class="
                          modal-content-container--header--buttons--button
                          modal-content-container--header--buttons--minimize
                          tooltip_vertical
                        "
            title="Minimize"
            @click.stop="closeModal" 
            v-html="CosmosIconPack('modalMinimize')"
          />
          <div
            class="
                          modal-content-container--header--buttons--button
                          modal-content-container--header--buttons--expand
                          tooltip_vertical
                        "
            title="Toggle maximization"
            :class="{ 'maximized': modalMaximized }"
            @click.stop="modalMaximized = !modalMaximized" 
            v-html="CosmosIconPack('modalExpand')"
          />
        </div>
      </div>
      <slot class="modal-content" />
    </div>
  </div>
</template>

<script>
import CosmosIconPack from '@/styles/CosmosIconPack.js';
import TooltipEquip from '@/styles/TooltipEquip.js';

import standardDimensions from '@/styles/_dimensions.scss';

let _instance_count = 0; // Total number of modal components in existence (used to make sure they don't overlap at all on instantiation)

export default {
    name: 'ModalComponentContainer',
    props: {
    // Text placed in the headed of the modal when displayed.
        headerText: {
            type: String,
            required: false,
            default: ''
        },
        // URI of Modal Container Icon, relative to public (shown when container is closed):
        drawerIcon: {
            type: String,
            required: false,
            default: 'commandLine'
        }
    },
    data(){
        return {
            CosmosIconPack,

            showModalContent: false, // Whether the modal content should be shown
            modalMaximized: false, // Whether the modal is maximized
            // Position data for modal container icon (serializable for storing and recovering from DB):
            modalContainerIconData: {
                top: '85%',
                side: 'left'
            },
            // Data about modal container drag:
            modalContainerDragData: {
                offset: { x: 0, y: 0 }, // Starting position of mouse when drag begins
            },

            emptyImage: new Image(), // Empty image (single transparent image), needs to be preloaded for certain drag elements
        };
    },
    mounted(){
        TooltipEquip(this.$el);
    },
    created() {
    // Make sure drawers don't overlap on instantiation (if there are multiple):
    // NOTE: FIXME: This technically creates a risk of components leaving the screen by having a calc position < 0. This 
    // would only be an issue if a lot of drawer icons were used, which we're not planning on.
    // Could also (much sooner) put icons out of draggable range.
    // Also: consider alternating sides to more effectively use space.
        this.modalContainerIconData.top = `calc(85% - ${_instance_count * 1.5} * ${standardDimensions.CSModalDrawerIconSize})`;
        _instance_count++;

        // Preload Single Transparent Pixel (for making drag icon invisble):
        this.emptyImage.src = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNkYAAAAAYAAjCB0C8AAAAASUVORK5CYII=';

        // Close this modal if another modal is trying to open on this side:
        this.$eventHub.$on('INFRA-closeModalsOnSide', this.closeModalBySide);
    },
    beforeDestroy() {
        this.$eventHub.$off('INFRA-closeModalsOnSide', this.closeModalBySide);
    },
    methods: {
    /**
     * Opens this modal when the drawer icon is clicked but first emits an event to close any modals currently open on the same side.
     */
        openModal(){
            this.$eventHub.$emit('INFRA-closeModalsOnSide', this.modalContainerIconData.side);
            this.showModalContent = true;
        },
        /**
     * Closes this modal.
     */
        closeModal(){
            this.showModalContent = false;
            this.modalMaximized = false;
        },

        /**
     * Closes this modal if it's on the given side.
     */
        closeModalBySide(side){ 
            if(this.showModalContent && this.modalContainerIconData.side === side){
                this.closeModal();
            }
        },

        /** -- MODAL DRAG FUNCTIONS --  **/

        // Update Data at Start of Modal Container Drag:
        modalDragStart(event){
            const target = event.target.closest('.modal-container');
            this.modalContainerDragData.offset.x = target.offsetLeft - event.clientX;
            this.modalContainerDragData.offset.y = target.offsetTop - event.clientY;

            event.dataTransfer.setDragImage(this.emptyImage,0,0); // Prevent ghost image from showing on drag
        },
        // Update Modal Container during Drag:
        modalOnDrag(event){
            // If not the final @drag call (in which (x,y) is set to (0,0)):
            if(!(event.clientX===0 && event.clientY===0)){
                let y = this.modalContainerDragData.offset.y + event.clientY;

                // Ensure this won't send the container contents out of bounds:
                const height = parseFloat(standardDimensions.CSModalHeight);

                if(y > document.body.clientHeight){
                    y = document.body.clientHeight;
                } else if( y - height < 0 ){
                    y = height;
                }

                this.modalContainerIconData.top = `${y}px`;

                // If within 1/3 of a wall, snap to wall:
                const snapDist = 1/3;
                if(event.clientX < snapDist*document.body.clientWidth){
                    this.changeSide('left');
                } else if(event.clientX > (1-snapDist)*document.body.clientWidth){
                    this.changeSide('right');
                }
            }
        },

        /**
     * Changes the side to which this modal / modal drawer is docked to the given side 
     * and, if this modal is open, it tells all other opened modals on the target side 
     * to close.
     */
        changeSide(side){
            if(this.modalContainerIconData.side !== side){
                if(this.showModalContent){
                    this.$eventHub.$emit('INFRA-closeModalsOnSide', side);
                }
                this.modalContainerIconData.side = side;
            }
        }
    }
};
</script>

<style scoped lang="scss">
@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';
@import '@/styles/_dimensions.scss';

  // Rendering Order:
  $z-index-modal: 9000000;
  $z-index-icon: 9000010;
  $z-index-modal-maximized: 9000020;

  @mixin shadow($scale){
    box-shadow: 0 0 $scale*2rem $scale*0.5rem $color-near-black;
  }

  .modal-container {
    position: absolute;

    @mixin dock-side(){
      &.left {
        left: 0;
      }
      &.right {
        right: 0;
      }
    }
    @include dock-side();

    &.maximized {
      top: $std-tab-height + 2 * $std-spacing;
      bottom: 2 * $std-spacing;
    }

    & > &--icon {
      z-index: $z-index-icon;

      @include dock-side();
      @include shadow(0.2);
      
      position: absolute;
      bottom: 0;
      
      background: $color-primary;
      stroke-width: 1px;

      padding: 4px; // Shrinks the size of the contained icon
      width: $std-cs-modal-drawer-icon-size;
      height: $std-cs-modal-drawer-icon-size;
    }
    
    & > .modal-content-container {
      @include dock-side();
      @include shadow(1);

      $min-width: $std-cs-modal-width;
      min-width: $min-width;

      position: absolute;
      bottom: 0;

      &.maximized {
        z-index: $z-index-modal-maximized;
        width: 33vw;
        height: 100%;
      }
      &:not(.maximized) {
        z-index: $z-index-modal;
        width: $min-width;
        height: $std-cs-modal-height;
      }

      margin: {
        left: 2 * $std-spacing;
        right: 2 * $std-spacing;
      }

      display: grid;
      grid-template-columns: 1fr;
      grid-template-rows: min-content minmax(0, 1fr);

      & > .modal-content-container--header {
        width: 100%;
        height: .8 * $std-tab-height;

        background: $color-grey3;
        color: $color-near-white;

        display: flex;
        flex-direction: row;
        justify-content: space-between;
        align-items: center;

        & > .modal-content-container--header--title {
          padding: {
            top: 1rem;
            left: 1rem;
            bottom: 1rem;
          }
          line-height: 0;
        }

        & > .modal-content-container--header--buttons {
          flex-shrink: 0;

          display: grid;
          grid-template-rows: minmax(0, 1fr);
          grid-template-columns: minmax(0, 1fr);

          & > .modal-content-container--header--buttons--button {
            grid-row: 1;
            line-height: 0;
            width: 12px;
            height: 12px;
            margin: {
              right: 1rem;
            }

            stroke: $color-near-white;
            &:hover {
              stroke: $color-grey2;
            }

            transition: transform 0.3s;
          }
      
          & > .modal-content-container--header--buttons--expand {
            &.maximized {
              transform: rotate(180deg);
            }
          }
        }
      }
    }
  }
</style>
