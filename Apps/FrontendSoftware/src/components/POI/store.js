import {testPOIList} from "@/components/POI/testPOIData.js";

export default {
  state: {
    POIList: testPOIList,
    POIImageSelected: {
      POICard: null,
      imageIndex: -1,
    }
  },
  getters: {
    POIList: state => {
      return state.POIList.list;
    },
    POIImageSelected: state => {
      return state.POIImageSelected;
    },
    POIByTagList: state => {
      return state.POIList.POIByTagList;
    }
  },
  mutations: {
    deletePOI(state, POI) {
      state.POIList.deletePOI(POI);
    },

    updatePOICategory(state, {POI, newCategory}) {
      state.POIList.updatePOICategory(POI, newCategory);
    },     

    setPOIImageSelected(state, {POICard, imageIndex}) {
      state.POIImageSelected = {
        POICard: POICard,
        imageIndex: imageIndex
      };
    }
  }
};
