// Resources
// Dragging info: https://usefulangle.com/post/1/jquery-dragging-image-within-div

<template>
  <div class="modal-backdrop" v-on:click.self="closeModal">

    <div id="POIModalDetailsContainer">
      <div id="edit-thumbnail-backdrop" v-if="edittingThumbnail"></div>
      <h3 class="heading-poi">ADD NEW POI</h3>

      <div class="flex-container modal-content">

        <div class="modal-col">
          <div class="modal-row flex-container">

            <div class="pri-type">
              <div class="label">Priority</div>
              <Dropdown
                :options="['Low', 'Medium', 'High']"
                :default="'Medium'"
                class="select"
              />
            </div>

            <!-- <span class="dot-icon red"></span> -->
            <div class="pri-type">
              <div class="label">Type</div>
              <Dropdown
                :options="['Attraction', 'Obstacle', 'Shadow']"
                :default="parentData"
                class="select"
              />
              <!-- @input="alert(displayToKey($event))" -->
            </div>
          </div>

          <div class="modal-row">
            <div class="label">Name*</div>
            <input type="text" readonly placeholder="OBST-005" />
            <div class="sublabel">* Automatically Populated</div>
          </div>
          <div class="modal-row">
            <div class="label">Size</div>
            <div class="flex-container vertically-center">
              <!-- WIDTH -->
              <div class="dimension">
                <div class="dimen">Width (cm)</div>
                <input
                  type="text"
                  @keydown.space.prevent
                  @keypress="onlyNumber"
                  placeholder="Width"
                />
              </div>

              <svg
                class="x-icon"
                width="10"
                height="11"
                viewBox="0 0 10 11"
                fill="none"
                xmlns="http://www.w3.org/2000/svg"
              >
                <path d="M1 1L9 10" stroke="white" />
                <path d="M8.75555 1L1.24445 10" stroke="white" />
              </svg>

              <!-- HEIGHT -->
              <div class="dimension">
                <div class="dimen">Height (cm)</div>
                <input
                  type="text"
                  @keydown.space.prevent
                  @keypress="onlyNumber"
                  placeholder="Height"
                />
              </div>

              <svg
                class="x-icon"
                width="10"
                height="11"
                viewBox="0 0 10 11"
                fill="none"
                xmlns="http://www.w3.org/2000/svg"
              >
                <path d="M1 1L9 10" stroke="white" />
                <path d="M8.75555 1L1.24445 10" stroke="white" />
              </svg>

              <!-- DEPTH -->
              <div class="dimension">
                <div class="dimen">Depth (cm)</div>
                <input type="text" @keydown.space.prevent @keypress="onlyNumber" placeholder="Depth" />
              </div>
            </div>
          </div>

          <div class="modal-row">
            <div class="label">Details</div>
            <textarea rows="6" placeholder="Fill out details here"></textarea>
          </div>
        </div>

        <!-- Second Column: thumbnail image -->
        <div class="modal-col">
          <div class="flex-container buttons">
            Thumbnail photo
            <div class="edit-img-buttons-container" v-if="edittingThumbnail">
              <button class="button modal-button thumbnail-btn" v-on:click="hideEditButtons">
                CANCEL
              </button>
              <button class="button modal-button done thumbnail-btn">DONE</button>
            </div>
          </div>

          <span v-on:click="editThumbnail"
            ><EditPencilIcon class="edit-pencil-icon"
          /></span>
          <img
            class="thumbnail"
            v-bind:style="[thumbnailBorder]"
            draggable="false"
            src="@/assets/imgviewer/POI_test_image.png"
          />
        </div>
      </div>

      <div id="btn-container">
        <button class="button modal-button" v-on:click="closeModal">
          CANCEL
        </button>
        <button class="button modal-button next">NEXT</button>
      </div>
    </div>
  </div>
</template>

<script>
import Dropdown from "@/components/POI/Components/Dropdown.vue";
import EditPencilIcon from "@/assets/imgviewer/SVGcomponents/EditPencilIcon.vue";

export default {
  name: "POIModalFullDetails",

  props: {
    parentData: String,
  },

  components: {
    Dropdown,
    EditPencilIcon,
  },

  data() {
    return {
      thumbnailBorder: {
        borderRadius: "0.4rem",
        cursor: "auto",
      },
      edittingThumbnail: false,
    };
  },

  methods: {
    closeModal() {
      this.$emit("closeTheModal");
    },

    onlyNumber($event) {
      let keyCode = $event.keyCode ? $event.keyCode : $event.which;
      if ((keyCode < 48 || keyCode > 57) && keyCode !== 46) {
        // 46 is dot
        $event.preventDefault();
      }
    },

    editThumbnail() {
      this.thumbnailBorder.borderRadius = "0rem";
      this.thumbnailBorder.cursor = "move";
      this.edittingThumbnail = true;
      
    },

    hideEditButtons() {
      this.edittingThumbnail = false;
      this.thumbnailBorder.cursor = "auto";
    },
  },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";
@import "@/styles/_functional.scss";

.modal-backdrop {
  position: fixed;
  top: 0;
  bottom: 0;
  left: 0;
  right: 0;
  background-color: rgba(0, 0, 0, 0.8);
  display: flex;
  justify-content: center;
  align-items: center;
  z-index: 2;
}

/* REM conversion: 1rem = 10px */
#POIModalDetailsContainer {
  background: #2a2a2a;
  overflow-x: auto;
  position: fixed;
  width: 52.8rem;
  border-radius: 0.4rem;
  padding: 2rem;

  font-size: 1rem;
  font-style: normal;
  font-weight: 400;
  text-align: left;
  z-index: 2;
}

#edit-thumbnail-backdrop{
  position: absolute;
  width: 100%;
  height: 100%;
  background-color: #2A2A2A;
  opacity: .7;
  margin: -2rem;
  z-index: 3;
}

.modal-content {
  margin-bottom: 4rem;
}

.flex-container {
  display: flex;
}

.modal-col {
  flex: 0 0 50%;
}

.modal-col:first-child {
  padding-right: 2rem;
}

.modal-col:last-child {
  z-index: 3;
}

.modal-row {
  padding: 0.8rem 0;
}

.modal-row:first-child {
  padding-top: 0;
}

.modal-row:last-child {
  padding-bottom: 0;
}

.heading-poi {
  padding: 0 0 2rem 0;
}

.select{
  z-index: 2;
}

.pri-type {
  flex: 0 0 50%;
  padding: 0 0.8rem;
}

.pri-type:first-child {
  padding-left: 0;
}

.pri-type:last-child {
  padding-right: 0;
}

.label {
  margin-bottom: 0.8rem;
}

.vertically-center {
  align-items: flex-end;
}

.dimen {
  font-size: 0.8rem;
  margin-bottom: 0.4rem;
}

.x-icon {
  // align-items: center;
  margin-bottom: 0.4rem;
}

.sublabel {
  padding-top: 0.4rem;
  font-size: 0.7rem;
  font-weight: 400;
  text-align: left;
  color: #9f9f9f;
}

.dimension {
  padding: 0 0.8rem;
}

.dimension:first-child {
  padding-left: 0;
}

.dimension:last-child {
  padding-right: 0;
}

.dropdwn{

}

.thumbnail {
  width: 100%;
  top: 100%;
  object-fit: cover;
}

input,
textarea {
  background-color: $color-grey-dark;
  border: none;
  border-radius: 0.4rem;
  font-family: inherit;
  width: 100%;
  font-size: 10px;
  font-style: normal;
  font-weight: 400;
  padding: 0.4rem;
  color: #fcfcfc;

  &::-webkit-input-placeholder {
    color: #9f9f9f;
  }

  &::-moz-placeholder {
    color: #9f9f9f;
  }

  &::-ms-placeholder {
    color: #9f9f9f;
  }

  &::placeholder {
    color: #9f9f9f;
  }

  &:focus {
    outline: none !important;
  }
}

input {
  padding-left: 0.6rem;
}

textarea {
  overflow-y: scroll;
  resize: none;
  width: 100%;
}

// Buttons
.modal-button {
  font-size: 0.95rem;
  font-weight: 600;
  text-align: center;
  width: 6.5rem;
}

.next,
.done {
  background-color: $color-primary;
  border-color: $color-primary;
  margin-left: 0.8rem;
}

.flex-container .buttons{
  justify-content: space-between;
  margin-bottom: .8rem;
  align-items: flex-end;
}

.edit-img-buttons-container{
  position: absolute;
  right: 2rem;
}

#btn-container{
  float: right;
}

// Icons
.dot-icon {
  width: 0.8rem;
  height: 0.8rem;
  top: 0.2rem;
  display: inline-block;
  margin-right: 0.8rem;
  margin-top: 0.1rem;
  border-radius: 50%;
}

.dot-icon.red {
  background-color: #ff2f46;
}

.dot-icon.green {
  background-color: #21df9a;
}

.edit-pencil-icon {
  position: absolute;
  margin-top: 1.6rem;
  right: 3.6rem;
  width: 1.2rem;
  cursor: pointer;
}
</style>