import LocalizationData from "@/data_classes/LocalizationData.js";

class LocalizationTestList {
  constructor() {
    this._list = [new LocalizationData(
      { // data which gets saved to JSON
        lookupID: 1, // DB ID of this packet
        collectionTime: new Date(), // Time the source data (image) was collected.
        sendTime: new Date(), // Time this packet was completed and pushed to DB.
  
        // Note: for positions x+ points to the right on the map and y+ points up.
        position: [10, 10, -30], // Position estimate [x,y, heading]
        uncertainty: [0,0,0], // Uncertainty in the measurement [+-x, +-y, +-angle]
  
        // These fields are only used if this packet corresponds to / is produced
        // from an image. commandLookupID is 0 if this data did not come while any
        // commands were being executed.
        commandLookupID: -1, // ID of Command which put the rover in this state
  
        // Moves longer than frame.len will get broken up into multiple smaller
        // moves for a breadcrumb image to be taken after each one.
        // Note: n and tot count from 1. n is 0 if this data does not correspond
        // with any image-bearing point (ie. end of a command or a breadcrumb frame)
        frame: {
          len: 10, // [cm] Length of segmenting
          n: 0, // Frame number corresponding to this data point
          tot: 0 // Total number of frames in this move
        }
    })];
    this._tick = 3000;
    this.addData();
  }

  addData() {
    setTimeout(function() {
      let initX = this._list[0].data.position[0];
      let initY = this._list[0].data.position[1];
  
      let newX = initX + this.randomGen();
      let newY = initY + this.randomGen();
      let newAng = this.randomAngleGen();
  
      let newData = new LocalizationData(
        { 
          lookupID: 1, // DB ID of this packet
          collectionTime: new Date(), // Time the source data (image) was collected.
          sendTime: new Date(), // Time this packet was completed and pushed to DB.
    
          // Note: for positions x+ points to the right on the map and y+ points up.
          position: [newX, newY, newAng], // Position estimate [x,y, heading]
          uncertainty: [0,0,0], // Uncertainty in the measurement [+-x, +-y, +-angle]
    
          // These fields are only used if this packet corresponds to / is produced
          // from an image. commandLookupID is 0 if this data did not come while any
          // commands were being executed.
          commandLookupID: -1, // ID of Command which put the rover in this state
    
          // Moves longer than frame.len will get broken up into multiple smaller
          // moves for a breadcrumb image to be taken after each one.
          // Note: n and tot count from 1. n is 0 if this data does not correspond
          // with any image-bearing point (ie. end of a command or a breadcrumb frame)
          frame: {
            len: 10, // [cm] Length of segmenting
            n: 0, // Frame number corresponding to this data point
            tot: 0 // Total number of frames in this move
          }
      });
  
      this._list.unshift(newData);

    }.bind(this), this._tick);
  }

  randomGen() {
    const maxDist = 60;
    if (Math.random() > 0.5) {
      return -1 * Math.floor((Math.random() * maxDist) + 1);
    }
    return Math.floor((Math.random() * maxDist) + 1);
  }

  randomAngleGen() {
    const maxAngle = 360;
    if (Math.random() > 0.5) {
      return -1 * Math.floor((Math.random() * maxAngle) + 1);
    }
    return Math.floor((Math.random() * maxAngle) + 1);
  }

  getList() {
    return this._list;
  }
}

export default LocalizationTestList;