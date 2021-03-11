// Tests:
// import data from "@/components/Map/Test/TestData/TestFrames.js";
// import data from "@/components/Map/Test/TestData/TestExistingLookup.js";

const data = null;

class LocalizationTestList {
    constructor() {
        this._list = [];
        this._currIdx = 0;
        this._tick = 1000;
        if (!data) {return;}
        this.addData();
    }

    addData() {
        setInterval(function() {
            if (this._currIdx >= data.length) { return; }
            let nextData = data[this._currIdx];

            if (!this.replaceInList(nextData)) {
                // Push new data if lookupID doesn't exist in array
                this._list.push(nextData);
            }
            this._currIdx++;

            console.log(this._list.map(elem => elem.data.position));
        }.bind(this), this._tick);
    }

    // Replaces the obj if the lookupID exists
    replaceInList(nextData) {
        let idx = this._list.findIndex(elem => elem.data.lookupID === nextData.data.lookupID);
    
        // If index already exists, replace it with new obj
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