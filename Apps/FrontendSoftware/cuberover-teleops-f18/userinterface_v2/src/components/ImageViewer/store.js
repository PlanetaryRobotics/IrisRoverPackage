/*
 * Vuex Module for Splitting out the Aspects of the Central Store owned by the
 * Image Viewer.
 *
 * Author: John Walker Moosbrugger, CMU
 * Created: 3/28/19
 * Last Updated: 5/12/19, Colombo
 */

/* global __static */ // <- keep eslint from complaining about the __static directory
import path from 'path'
import fs from 'fs'

import ImageData from '@/data_classes/ImageData.js'
import Preset from '@/data_classes/Preset.js'
import Tag from '@/data_classes/Tag.js'

// Simulate the binary image data which will be Received from the DB:
function bytes(file){
  let data = fs.readFileSync(file);
  return Uint8Array.from(data);
}

export default {
  state: {
    // The scroll position for the timeline, also selects displayed image.
    scrollPos: 0,

    // State of the FunctionalAdjustments editor (adjustments and preset name).
    // See FunctionalAdjustments.vue#resetAdjustments for default content template.
    adjustmentsEditorState: {
      name: "",
      adjustments: {
        Denoise: 0,
        Exposure: 0,
        Contrast: 0,
        Highlights: 0,
        Shadows: 0,
        Whites: 0,
        Blacks: 0
      }
    },

    // Images synced from MongoDB: NEEDS TO BE DYNAMIC
    images: [
      new ImageData({
        name: 'CR_M01_0000',
        bin: bytes(path.join(__static,'./images/NFwvadh.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: ["Nathan"]
      }),
      new ImageData({
        name: 'CR_M01_0001',
        bin: bytes(path.join(__static,'./images/Kd2V45y.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "Forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0002',
        bin: bytes(path.join(__static,'./images/uJg1pom.jpg')),
        camera: "Rear",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0003',
        bin: bytes(path.join(__static,'./images/OUuN3i4.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0004',
        bin: bytes(path.join(__static,'./images/E9DbeBD.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0005',
        bin: bytes(path.join(__static,'./images/466fWFX.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0006',
        bin: bytes(path.join(__static,'./images/FBig7ow.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0007',
        bin: bytes(path.join(__static,'./images/MTipXK1.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0008',
        bin: bytes(path.join(__static,'./images/dICMJTn.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0009',
        bin: bytes(path.join(__static,'./images/hOhhfSy.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0010',
        bin: bytes(path.join(__static,'./images/NwiaJX0.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0011',
        bin: bytes(path.join(__static,'./images/XT8kVRV.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0012',
        bin: bytes(path.join(__static,'./images/fRxsqv3.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0013',
        bin: bytes(path.join(__static,'./images/je839X2.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0014',
        bin: bytes(path.join(__static,'./images/GKgsaJn.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0015',
        bin: bytes(path.join(__static,'./images/DhsYo0S.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0016',
        bin: bytes(path.join(__static,'./images/aiseH8z.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0017',
        bin: bytes(path.join(__static,'./images/xsyv7eX.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
      new ImageData({
        name: 'CR_M01_0018',
        bin: bytes(path.join(__static,'./images/rxpq6t5.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: []
      }),
    ],

    // Stores bool for if user is creating new tag
    isAddTag: false,
    // TAGS
    Tags: [
      new Tag({name: "Squid", initialImage:"CR_M01_0001", tagNames:[]}),
      new Tag({name: "Rock", initialImage:"CR_M01_0001", tagNames:[]}),
      new Tag({name: "Rocco", initialImage:"CR_M01_0001", tagNames:[]}),
      new Tag({name: "Steve", initialImage:"CR_M01_0000", tagNames:[]})
    ],

    searchedTags: [],

    // PRESETS
    Presets: [
      new Preset({
        name: "Preset1",
        adjustments: {
          Exposure: -.5,
          Contrast: .2,
          Denoise: 0,
          Highlights: .2,
          Shadows: -.1,
          Whites: 0,
          Blacks: .3,
        },
        initialImage: "CR_M01_0001"
      })
    ]
  },

  mutations: {
    // Resets the default state for the adjustments editor
    RESET_ADJUSTMENTS(state){
      state.adjustmentsEditorState = {
        name: "",
        adjustments: {
          Exposure: 0,
          Contrast: 0,
          Denoise: 0,
          Highlights: 0,
          Shadows: 0,
          Whites: 0,
          Blacks: 0
        }
      };
    },
    SCROLL_BACK (state) {
      state.scrollPos++
    },
    SCROLL_FORWARD (state) {
      state.scrollPos--
    },

    SET_SCROLL (state, imgPos) {
      state.scrollPos = imgPos
    },

    CREATE_PRESET (state, inputData) {
      state.Presets.push( new Preset(inputData) )
    },

    CREATE_TAG (state, inputData) {
      state.Tags.push( new Tag(inputData) )
      state.tagImg= ""
      state.isAddTag = false
    },

    ADD_TAG (state) {
      state.isAddTag = true
    },

    CANCEL_ADD_TAG (state) {
      state.isAddTag = false
    },

    SEARCH_TAG (state, tagName) {
      state.searchedTags.push(tagName)
    },

    UNSEARCH_TAG (state, tagName) {
      state.searchedTags.splice( state.searchedTags.indexOf(tagName), 1 )
    },

    TOGGLE_TAG (state, inputData) { // Toggles the tag on a specific image
      for (const tag of state.Tags) {
        if (tag.getName() === inputData.tagName) {
          tag.toggleImage(inputData.imgName)
        }
      }
    },

    TOGGLE_PRESET_VISIBILITY (state, inputData) {
      for (let index in state.Presets) {
        if (state.Presets[index].name() === inputData.presetName) {
          state.Presets[index].toggleVisibility(inputData.currentImage)
        }
      }
    },

    TOGGLE_PRESET_GLOBAL (state, presetName) {
      for (let index in state.Presets) {
        if (state.Presets[index].name() === presetName) {
          state.Presets[index].toggleGlobal()
        }
      }
    }
  },

  getters: {
    tagNames: state => { // Returns list of the names of all tags
      const tagNames = state.Tags.map(x => x.getName())
      return tagNames
    },
    tagsOnImg: state => (imageName) => {
      let tagsOnImg = {}
      for (let tag in state.Tags) {
        let tagName = tag.getName()
        if (tag.getImages().includes(imageName)) {
          tagsOnImg[tagName] = true
        }
        else {
          tagsOnImg[tagName] = false
        }
      }
      return tagsOnImg
    },
    searchedImages: state => {
      let list = []
      let imageList = []
      if (state.searchedTags != "") {
        for (let tag of state.Tags) {
          if (state.searchedTags.includes(tag.getName())) {
            for (let image of tag.getImages()) {
              if (!(list.includes(image))) {
                list.push(image)
              }
            }
          }
        }
        for (let image of state.images) {
          if (list.includes(image.name())) {
            imageList.push(image)
          }
        }
      }
      else {
        imageList = state.images
      }
      return imageList
    }
  }
}
