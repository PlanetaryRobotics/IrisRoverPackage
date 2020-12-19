<template>
  <div class="modal-backdrop" v-on:click.self="closeModal">
    <div id="POIModalDetailsContainer">
      <h3 class="heading-poi">ADD NEW POI</h3>

      <div class="flex-container modal-content">
        <!-- First Column: form information -->
        <div class="modal-col">
          <!-- Left Column Form 1 -->
          <span v-show="formOneVisible">
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
              </div>
            </div>

            <div class="modal-row">
              <div class="label">Name*</div>
              <input type="text" readonly placeholder="OBST-005" />
              <div class="sublabel">* Automatically Populated.</div>
            </div>
            <div class="modal-row">
              <div class="label">Size</div>
              <div class="flex-container vertically-bottom">
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
                  <input
                    type="text"
                    @keydown.space.prevent
                    @keypress="onlyNumber"
                    placeholder="Depth"
                  />
                </div>
              </div>
            </div>

            <div class="modal-row">
              <div class="label">Details</div>
              <textarea rows="6" placeholder="Fill out details here"></textarea>
            </div>
          </span>

          <!-- Form 2-- hidden until next button clicked on form 1 modal -->
          <span v-show="formTwoVisible">
            <div class="modal-row">
              <div class="label">Tags</div>
              <div
                class="add-tag-input-container flex-container vertically-center"
              >
                <input
                  class="add-tag-input"
                  @keydown.space.prevent
                  type="text"
                  v-on:input="tagUserInput = $event.target.value"
                  placeholder="Search"
                />
                <div
                  class="addTagButton flex-container vertically-center"
                  v-on:click="createNewTag"
                >
                  <WhiteAddIcon class="white-add-icon" />
                </div>
              </div>
            </div>

            <div class="modal-row">
              <div id="tag-pill-container"></div>
            </div>
          </span>
        </div>

        <!-- Second Column: thumbnail image -->
        <div class="modal-col">
          <div class="label">Thumbnail Photo</div>
          <!-- <canvas id="imgCanvas" /> -->
          <img
            class="thumbnail"
            draggable="false"
            :src="imgData"
            alt="Selected area of image not loaded"
          />
          <!-- <img
            class="thumbnail"
            draggable="false"
            src="@/assets/imgviewer/POI_test_image.png"
          /> -->
        </div>
      </div>

      <div id="btn-container">
        <!-- Form 1 Buttons -->
        <span v-if="formOneVisible">
          <button class="button modal-button" v-on:click="closeModal">
            CANCEL
          </button>
          <button
            class="button modal-button next purple"
            v-on:click="populateAsFormTwo"
          >
            NEXT
          </button>
        </span>

        <!-- Form 2 Buttons -->
        <span v-if="formTwoVisible">
          <button class="button modal-button" v-on:click="closeModal">
            CANCEL
          </button>

          <button
            class="button modal-button purple previous"
            v-on:click="populateAsFormOne"
          >
            PREVIOUS
          </button>

          <button class="button modal-button purple create">CREATE</button>
        </span>
      </div>
    </div>

    <AddTagModalWithinPOIModal
      v-if="addTagModalVisible"
      v-on:closeModal="closeAddTagModal"
      v-on:newTag="pushNewTag"
      v-bind:tagInput="tagUserInput"
    />
  </div>
</template>

<script>
import Dropdown from "@/components/POI/Components/Dropdown.vue";
import WhiteAddIcon from "@/assets/imgviewer/SVGcomponents/WhiteAddIcon.vue";
import AddTagModalWithinPOIModal from "@/components/POI/Components/AddTagModalWithinPOIModal.vue";
import { mapGetters } from "vuex";
import getPixels from "get-pixels";
import util from "util";
import savePixels from "save-pixels";
import streamConcat from "concat-stream";

// Promisify function for image data getters in computed
const getPromisedPixels = util.promisify(getPixels);

export default {
  name: "POIModalFullDetails",

  props: {
    parentData: String,
  },

  components: {
    Dropdown,
    WhiteAddIcon,
    AddTagModalWithinPOIModal,
  },

  data() {
    return {
      mouseDownThumbnail: false,
      formOneVisible: true,
      formTwoVisible: false,
      tagUserInput: "",
      chosenTagList: [{ details: "Details test", name: "testing" }],
      addTagModalVisible: false,
    };
  },

  methods: {
    closeModal() {
      this.$emit("closeTheModal");
    },

    closeAddTagModal() {
      this.addTagModalVisible = false;
    },

    // Push new tag from tag creation modal to chosen tag list
    pushNewTag(tag) {
      // Push new tag to array of chosen tags
      this.chosenTagList.push(tag);
    },

    onlyNumber($event) {
      let keyCode = $event.keyCode ? $event.keyCode : $event.which;
      if ((keyCode < 48 || keyCode > 57) && keyCode !== 46) {
        // 46 is dot
        $event.preventDefault();
      }
    },

    populateAsFormOne() {
      // Show form 1
      this.formOneVisible = true;

      // Hide form 2
      this.formTwoVisible = false;
    },

    populateAsFormTwo() {
      // Hide form 1 divs in left column
      this.formOneVisible = false;

      // Show form 2 divs in left column
      this.formTwoVisible = true;
      // Auto populate tags visible if any already created (e.g. if back button pushed then next pushed again)
      this.updateTagPills();
    },

    createNewTag() {
      this.addTagModalVisible = true;
    },

    updateTagPills() {
      // clear current tags
      document.getElementById("tag-pill-container").innerHTML = "";

      // for tag in this.chosenTagList: create new tag pill so updates when tags deleted and created
      for (var i = 0; i < this.chosenTagList.length; i++) {
        let arr = this.chosenTagList[i];

        let tagDiv = document.createElement("div");
        tagDiv.setAttribute("class", "tag-pill");
        tagDiv.innerHTML = arr.name + " ";

        // create cross image
        var svg = document.createElementNS("http://www.w3.org/2000/svg", "svg");
        svg.setAttribute("width", "8");
        svg.setAttribute("height", "8");
        svg.setAttribute("fill", "none");
        svg.setAttribute("viewbox", "0 0 8 8");
        svg.setAttribute("class", "x-tag-pill");

        var path1 = document.createElementNS(
          "http://www.w3.org/2000/svg",
          "path"
        );
        path1.setAttribute(
          "d",
          "M7.23045 0.980647C7.31181 0.899288 7.31181 0.767379 7.23045 0.686019C7.14909 0.60466 7.01718 0.60466 6.93582 0.686019L3.95815 3.66369L0.980484 0.686025C0.899125 0.604666 0.767215 0.604666 0.685856 0.686025C0.604497 0.767385 0.604497 0.899294 0.685856 0.980653L3.66352 3.95832L0.683822 6.93802C0.602463 7.01938 0.602463 7.15129 0.683822 7.23265C0.765181 7.31401 0.897091 7.31401 0.97845 7.23265L3.95815 4.25295L6.93786 7.23266C7.01922 7.31401 7.15113 7.31401 7.23249 7.23266C7.31385 7.1513 7.31385 7.01939 7.23249 6.93803L4.25278 3.95832L7.23045 0.980647Z"
        );
        path1.setAttribute("fill", "#FCFCFC");

        svg.appendChild(path1);

        svg.setAttributeNS(
          "http://www.w3.org/2000/xmlns/",
          "xmlns:xlink",
          "http://www.w3.org/1999/xlink"
        );

        svg.onclick = () => {
          this.removeTag(arr);
        };

        tagDiv.appendChild(svg);
        document.getElementById("tag-pill-container").appendChild(tagDiv);
      }
    },

    removeTag(tagVal) {
      for (var i = 0; i < this.chosenTagList.length; i++) {
        if (this.chosenTagList[i] == tagVal) {
          this.chosenTagList.splice(i, 1);
          break;
        }
      }
    },
  },

  watch: {
    chosenTagList: function () {
      this.updateTagPills();
    },
  },

  computed: {
    // Manual POI Pop Up
    ...mapGetters({
      selectedImage: "selectedImage",
      POIStartCoords: "POIStartCoords",
      POIEndCoords: "POIEndCoords",
    }),
  },

  asyncComputed: {
    async imgData() {
      // let POILayerDiv = document.getElementById("featurevp");

      let rowStart = Math.min(this.POIStartCoords[1], this.POIEndCoords[1]);
      let rowEnd = Math.max(this.POIStartCoords[1], this.POIEndCoords[1]);

      let colStart = Math.min(this.POIStartCoords[0], this.POIEndCoords[0]);
      let colEnd = Math.max(this.POIStartCoords[0], this.POIEndCoords[0]);

      let pixels = await getPromisedPixels(this.selectedImage.url);

      let pixelsWidth = pixels.shape[0];
      let pixelsHeight = pixels.shape[1];

      // "un"normailze coords
      rowStart = rowStart * pixelsHeight;
      rowEnd = rowEnd * pixelsHeight;
      colStart = colStart * pixelsWidth;
      colEnd = colEnd * pixelsWidth;

      let numCols = Math.abs(colEnd - colStart);
      let numRows = Math.abs(rowEnd - rowStart);
      let extraPadding = Math.abs(numRows - numCols) / 2;

      // shape imgCanvas to square
      document.getElementById("imgCanvas").width = numCols
      document.getElementById("imgCanvas").height = numRows

      // If col > row, add rows
      if (numCols > numRows) {
        rowStart = rowStart - extraPadding;
        rowEnd = rowEnd + extraPadding;
      }
      // If row > col, add cols
      else if (numRows > numCols) {
        colStart = colStart - extraPadding;
        colEnd = colEnd + extraPadding;
      }
      // visa versa, and will hit errors when edge cases (literally)
      // Use https://github.com/scijs/ndarray-concat-cols and https://github.com/scijs/ndarray-concat-rows
      // ndarray zeros

      // PSEUDO CODE
      // after creating row start, end, etc.
      // find selection width and height, if selWidth > height, make more rows to be square and visa versa
      // (selection width-selection height)/2
      // update col start and col end
      // decrease col start (-) by (selection width-selection height)/2
      // increase col end + (selection width-selection height)/2

      // Edge cases
      // If you go under:
      // Is new col start < 0
      // If true: make a topoverflow to make for extra cols needed on top
      // top overflow should be equal to absolute val = (0-colStart)
      // bottom overflow = colEnd - pixels.height

      // *use concat rows in link above* (https://github.com/scijs/ndarray-concat-rows)
      // will have a matrix that's some width and some height and add a couple rows on top and bottom (this is for rare case where someone selects almost or fully entire image)
      // DO: concatrows with ndarray:
      // concatRows([zeros([pixelWidth, topoverflowRows, 4]), selection, zeros([pixelWidth, bottom overflow rows, 4]))
      // EXPLANATION: create something as wide as image and only as tall as overflow
      // and visa versa then stack them and 4 as placeholder for RGBA
      // selection is referring to image
      // set colend to height-1
      // set colstart to 0 if overflow on top
      // From there create 0 arrays from 0 function sent above and concat to stack them
      // Look into "pool".zero? https://github.com/scijs/ndarray-scratch#poolzerosshapedtype
      // can multiple by shades of gray if want but if all black then just 0s

      // all identical if switched for overflow on width just switch cols to rows and visa versa

      // console.log("Row start: ", rowStart, "\nRow end: ", rowEnd, "\nCol start: ", colStart, "\nCol end: ", colEnd)

      let croppedMatrix = pixels.hi(colEnd, rowEnd).lo(colStart, rowStart);

      let stream = savePixels(croppedMatrix, "png");

      return await new Promise((resolve, reject) => {
        stream.pipe(
          streamConcat((buf) => {
            console.log("yay");
            resolve("data:image/png;base64," + buf.toString("base64"));
          })
        );
        stream.on("error", (err) => {
          console.log("err");
          reject(err);
        });
      });
    },
  },
};

// Make img tag set src = imgDatafromcomputed like in imgviewersandbox
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
  z-index: 11;
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
  z-index: 11;
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
  z-index: 11;
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

#imgCanvas{
  position: absolute;
}

.heading-poi {
  padding: 0 0 2rem 0;
}

.select {
  z-index: 11;
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
  font-weight: 600;
}

.vertically-center {
  align-items: center;
}

.vertically-bottom {
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

// Thumbnail
.thumbnail {
  width: 100%;
  border-radius: 0.4rem;
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
  padding-left: 0.8rem;
}

.add-tag-input {
  padding-right: 2rem;
}

textarea {
  overflow-y: scroll;
  resize: none;
  width: 100%;
  padding-left: 0.8rem;
}

// Buttons
.modal-button {
  font-size: 0.95rem;
  font-weight: 600;
  text-align: center;
  width: 6.5rem;
}

.next,
.done,
.previous,
.create {
  margin-left: 0.8rem;
}

.purple {
  background-color: $color-primary;
  border-color: $color-primary;
}

.purple:hover {
  background-color: #6148c5;
  border-color: #6148c5;
}

.flex-container .buttons {
  justify-content: space-between;
  margin-bottom: 0.8rem;
  align-items: flex-end;
}

#btn-container {
  float: right;
}

// Tag pills on form 2: tags
#tag-pill-container {
  padding-top: 0.4rem;
  display: flex;
  flex-wrap: wrap;
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

.white-add-icon {
  position: relative;
  right: 0rem;
}

.add-tag-input-container {
  position: relative;
}

.addTagButton {
  padding: 0.3rem 0rem;
  position: absolute;
  right: 0.4rem;

  cursor: pointer;
}
</style>

<style>
/* unscoped style */
.tag-pill {
  margin: 0.4rem;
  margin-left: 0;
  padding: 0.4rem 0.8rem;
  border: 1px solid #585858;
  border-radius: 0.4rem;

  font-size: 1rem;
  color: #e9e9e9;
}

.x-tag-pill {
  margin-left: 0.4rem;
  cursor: pointer;
}
</style>