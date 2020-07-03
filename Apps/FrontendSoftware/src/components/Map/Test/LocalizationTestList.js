import data from "@/components/Map/Test/TestData/TestFrames.js";
//import data from "@/components/Map/Test/TestData/TestExistingLookup.js";

class LocalizationTestList {
  constructor() {
    this._list = [];
    this._currIdx = 0;
    this._tick = 1000;
    this.addData();
  }

  addData() {
    setInterval(function() {
      if (this._currIdx >= data.length) { return; }
      let nextData = data[this._currIdx];

      if (!this.replaceInList(nextData)) {
        this._list.push(nextData);
      }
      this._currIdx++;

      console.log(this._list.map(elem => elem.data.position));
    }.bind(this), this._tick);
  }

  replaceInList(nextData) {
    let idx = this._list.findIndex(elem => elem.data.lookupID === nextData.data.lookupID);
    if (idx !== -1) {
      this._list[idx] = nextData;
      return true;
    }

    return false;
  }

  getList() {
    return this._list;
  }
}

export default LocalizationTestList;