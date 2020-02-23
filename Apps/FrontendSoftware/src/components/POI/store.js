import {testPOIList} from "@/components/POI/testPOIData.js";

export default {
  state: {
    show: {
      showPOIManagement: true,
      showTagManagement: false,
    },

    POIListFreeze: testPOIList,
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
};
