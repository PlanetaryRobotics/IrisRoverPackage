/*
 * Vuex Module for Splitting out the Aspects of the Central Store owned by the
 * Image Viewer.
 *
 * Author: John Walker Moosbrugger, CMU
 * Created: 3/28/19
 * Last Updated: 10/7/19, Colombo
 */

// import fs from 'fs'

import DBLazyList from '@/DBInterface/DBLazyList.js';
import Collections from '@/DBInterface/Collections.js';

import ImageData from '@/data_classes/ImageData.js';
import Preset from '@/data_classes/Preset.js';
import Tag from '@/data_classes/Tag.js';

// Simulate the binary image data which will be Received from the DB:
// function bytes(file){
//   let data = fs.readFileSync(file);
//   return Buffer.from(Uint8Array.from(data));
// }

export default {
    state: {
        cartesianGrid: 0,
        radialGrid: 0,

        // The scroll position for the timeline (idx in the SEARCHED images array),
        scrollPos: 0,
        // LookupID of the displayed image:
        selectedImageLookupID: 1,

        // State of the FunctionalAdjustments editor (adjustments and preset name).
        // See FunctionalAdjustments.vue#resetAdjustments for default content template.
        adjustmentsEditorState: {
            name: '',
            adjustments: {
                Denoise: 0,
                Exposure: 0,
                Contrast: 0,
                Shadows: 0
            }
        },

        // List of ImageData Objects Held in the Log in the Database's Commands Collection:
        imageList: new DBLazyList({
            collection: Collections.Images,
            objClass: ImageData,
            headIdx: Infinity,
            length: Infinity,
            staleTime: Infinity
        }),

        // Stores bool for if user is creating new tag
        isAddTag: false,
        // TAGS
        Tags: [
            new Tag({name: 'Dwayne', tagNames:[]}),
            new Tag({name: 'Squid', tagNames:[]}),
            new Tag({name: 'Rock', tagNames:[]}),
            new Tag({name: 'Rocco', tagNames:[]}),
            new Tag({name: 'Steve', tagNames:[]})
        ],

        searchedTags: [],

        cameraSelection: 'Both', // Show Images from Both Cameras in TimelineSearch

        // PRESETS
        Presets: [
            new Preset({
                name: 'De-Bloom',
                adjustments: {
                    Exposure: -.48,
                    Contrast: .48,
                    Denoise: 0,
                    Shadows: 0
                },
                shared: true
            })
        ]
    },

    mutations: {
    // Resets the default state for the adjustments editor
        RESET_ADJUSTMENTS(state){
            state.adjustmentsEditorState = {
                name: '',
                adjustments: {
                    Exposure: 0,
                    Contrast: 0,
                    Denoise: 0,
                    Shadows: 0
                }
            };
        },

        // Sets the Scroll to Show the Image with the Given lookupID with Index pos
        // within the Selected (searched) Images Array:
        SET_SCROLL (state, {pos, lookupID}) {
            state.scrollPos = pos;
            state.selectedImageLookupID = lookupID;
        },

        CREATE_PRESET (state, inputData) {
            state.Presets.push( new Preset(inputData) );
        },

        CREATE_TAG (state, inputData) {
            state.Tags.push( new Tag(inputData) );
            state.tagImg= '';
            state.isAddTag = false;
        },

        ADD_TAG (state) {
            state.isAddTag = true;
        },

        CANCEL_ADD_TAG (state) {
            state.isAddTag = false;
        },

        SEARCH_TAG (state, tagName) {
            // Verify Tag Name is the Name of an Existing Tag:
            if(state.Tags.map(t => t.data.name).includes(tagName)){
                state.searchedTags.push(tagName); // Push to List
            }
        },

        UNSEARCH_TAG (state, tagName) {
            state.searchedTags.splice( state.searchedTags.indexOf(tagName), 1 );
        },

        TOGGLE_TAG (state, inputData) { // Toggles the tag on a specific image
            for (const tag of state.Tags) {
                if (tag.getName() === inputData.tagName) {
                    tag.toggleImage(inputData.imgName);
                }
            }
        },

        TOGGLE_PRESET_VISIBILITY (state, inputData) {
            for (let index in state.Presets) {
                if (state.Presets[index].name() === inputData.presetName) {
                    state.Presets[index].toggleVisibility(inputData.currentImage);
                }
            }
        },

        TOGGLE_PRESET_GLOBAL (state, presetName) {
            for (let index in state.Presets) {
                if (state.Presets[index].name() === presetName) {
                    state.Presets[index].toggleGlobal();
                }
            }
        }
    },

    getters: {
    // Returns a function to force vuex to recompute on each call
    // (otherwise it uses a cached value which only changes when the
    // /value/ of state.IMG.imageList changes):
        images: state => (() => state.imageList.list)(),
        // Selected Image being Displayed in the ImageViewport:
        selectedImage: (state, getters) => {
            if(getters.images.length){
                return getters.images.filter(i => i.data.lookupID == state.selectedImageLookupID)[0];
            } else{
                return new ImageData({}); // Return blank / default empty image.
            }
        },
        searchedImages: (state, getters) => {
            // TODO: This can be more efficient. That said, it's not even close to
            // being a bottleneck atm.

            // Filter to Select only Images Taken by the Selected Camera:
            let cameraFilter = i => state.cameraSelection == 'Both' || i.data.camera == state.cameraSelection;
            // Filter that will be used to select the searched images:
            let filter;

            if(state.searchedTags.length > 0){
                // Get Tag Data Associated with Each Tag Name by Filtering Tags down to
                // those which were Searched:
                let searchedTagData = state.Tags.filter( t => state.searchedTags.includes(t.data.name) );
                // Create a Set of All Unique Image Names belonging to the Given Tags:
                let imageNames = searchedTagData.reduce( (arr, t) => [...arr, ...t.data.images], []);
                imageNames = new Set(imageNames); // Eliminate duplicates
                imageNames = [...imageNames]; // Convert Set to Array for Later Filtering
                // Collect All Images with Name Associated with the Searched Tag and are
                // Taken by the Selected Camera:
                filter = i => cameraFilter(i) && [...imageNames].includes(i.data.name);
            } else{
                filter = i => cameraFilter(i);
            }

            // Select Only Given Images:
            let images = getters.images.filter( i => filter(i) );

            return images;
        },

        tagNames: state => { // Returns list of the names of all tags
            const tagNames = state.Tags.map(x => x.getName());
            return tagNames;
        },
        tagsOnImg: state => (imageName) => {
            let tagsOnImg = {};
            for (let tag in state.Tags) {
                let tagName = tag.getName();
                if (tag.getImages().includes(imageName)) {
                    tagsOnImg[tagName] = true;
                }
                else {
                    tagsOnImg[tagName] = false;
                }
            }
            return tagsOnImg;
        }
    }
};
