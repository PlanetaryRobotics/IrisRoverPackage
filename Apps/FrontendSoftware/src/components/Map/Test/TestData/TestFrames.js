import LocalizationData from "@/data_classes/LocalizationData.js";

const data = [
    new LocalizationData({
      lookupID: 1, // DB ID of this packet
      collectionTime: 0, // Time the source data (image) was collected.
      sendTime: 21, // Time this packet was completed and pushed to DB.
      position: [0,0,0], // Position estimate [x,y, heading]
      uncertainty: [0,0,0], // Uncertainty in the measurement [+-x, +-y, +-angle]
      commandLookupID: 1, // ID of Command which put the rover in this state
      frame: {
        len: 0, // [cm] Length of segmenting (rover didn't move any distance between frames (special case here for the deploy command since it's a non-move command which generates an image)
        n: 1, // Frame number corresponding to this data point (frame 1/1)
        tot: 1 // Total number of frames in this move (only 1 image gets taken)
      }
    }),

    // deadreckoning update - collected 2s into rover movement after first packet is sent
    new LocalizationData({
      lookupID: 2,
      collectionTime: 152, // time data was collected (on rover)
      sendTime: 160, // time data packet was finished being processed by localization and sent from backend to frontend
      position: [0, 60, 0], // cm, cm, deg
      uncertainty: [0.3, 1.0, 0.3], // cm, cm, deg
      commandLookupID: 2,
      frame: {
        n: 0, // this data isn't associated with an image frame
      }
    }),

    // deadreckoning update - collected 4s into rover movement  
    new LocalizationData({
      lookupID: 3,
      collectionTime: 154, // time data was collected (on rover)
      sendTime: 162, // time data packet was finished being processed by localization and sent from backend to frontend
      position: [-30, 60, -90], // cm, cm, deg
      uncertainty: [0.3, 1.5, 0.5], // cm, cm, deg
      commandLookupID: 3, // still associated with the same command
      frame: {
        n: 0, // this data isn't associated with an image frame
      }
    }),

    // deadreckoning update - collected 6s into rover movement 
    new LocalizationData({
      lookupID: 4,
      collectionTime: 156, // time data was collected (on rover)
      sendTime: 164, // time data packet was finished being processed by localization and sent from backend to frontend
      position: [-30, 90, 0], // cm, cm, deg
      uncertainty: [0.4, 2.0, 0.8], // cm, cm, deg
      commandLookupID: 3, // still associated with the same command
      frame: {
        n: 0, // this data isn't associated with an image frame
      }
    }),

    new LocalizationData({
      lookupID: 5,
      collectionTime: 153, // time data was collected (on rover)
      sendTime: 181, // time data packet was finished being processed by localization and sent from backend to frontend
      position: [-15, 75, 0], // cm, cm, deg
      uncertainty: [0.1, 0.1, 0.2], // cm, cm, deg
      commandLookupID: 3, // still associated with the same command
      frame: {
        len: 10, // [cm] Length of segments
        n: 1, // Frame number corresponding to this data point (frame 2 of 3)
        tot: 2 // Total number of frames in this move
      }
    }),

    new LocalizationData({
      lookupID: 6,
      collectionTime: 150, // time data was collected (on rover)
      sendTime: 181, // time data packet was finished being processed by localization and sent from backend to frontend
      position: [-15, 30, 0], // cm, cm, deg
      uncertainty: [0.1, 0.1, 0.2], // cm, cm, deg
      commandLookupID: 3, // still associated with the same command
      frame: {
        len: 10, // [cm] Length of segments
        n: 2, // Frame number corresponding to this data point (frame 2 of 3)
        tot: 2 // Total number of frames in this move
      }
    }),
];

export default data;