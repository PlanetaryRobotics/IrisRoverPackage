import {testPOIList} from "@/components/POI/testPOIData.js";

export default {
  state: {
    show: {
      showPOIManagement: true,
      showTagManagement: false,
    },
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
    showPOIManagement: state => {
      return state.show.showPOIManagement;
    },
    showTagManagement: state => {
      return state.show.showTagManagement;
    },
    POIImageSelected: state => {
      return state.POIImageSelected;
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
