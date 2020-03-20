import {testPOIList} from "@/components/POI/testPOIData.js";

export default {
  state: {
    show: {
      showPOIManagement: true,
      showTagManagement: false,
    },
    POIList: testPOIList
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
    }
  },
  mutations: {
    deletePOI(state, POI) {
      state.POIList.deletePOI(POI);
    },

    updatePOICategory(state, {POI, newCategory}) {
      state.POIList.updatePOICategory(POI, newCategory);
    }
  }
};
