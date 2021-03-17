import { testPOIList } from '@/components/POI/testPOIData.js';

export default {
    state: {
        POIList: testPOIList,
        POIImageSelected: {
            image: null,
            id: null,
        },
        TagSelected: {
            tag: null,
            id: null
        },
    },
    getters: {
        POIList: state => {
            return state.POIList.list;
        },
        POIByTagList: state => {
            return state.POIList.POIByTagList;
        },
        POIImageSelectedId: state => {
            return state.POIImageSelected.id;
        },
        POIImageSelected: state => {
            return state.POIImageSelected;
        },
        TagSelected: state => {
            return state.TagSelected;
        }
    },
    mutations: {
        deletePOI(state, POI) {
            state.POIList.deletePOI(POI);
        },

        updatePOICategory(state, { POI, newCategory }) {
            state.POIList.updatePOICategory(POI, newCategory);
        },

        updatePOIImportanceLevel(state, { POI, newImportance }) {
            state.POIList.updatePOICategory(POI, newImportance);
        },

        updatePOIImageSelectedId(state, { id, image }) {
            let obj = {
                image: image,
                id: id
            };
            state.POIImageSelected = obj;
        },

        updateTagSelected(state, { tag, id }) {
            state.TagSelected = { tag: tag, id: id };
        }
    }
};
