import LocalizationData from '@/data_classes/LocalizationData.js';

const data = [
    [ 
    // At t=0, Command 1 Deploy is sent to the rover.
    // Rover Deploys at 0,0 (by definition). 
    // Rover takes an image and the following gets pushed to Collections.Localiation:

        new LocalizationData({
            lookupID: 1, // DB ID of this packet
            collectionTime: 0, // Time the source data (image) was collected.
            sendTime: 21, // Time this packet was completed and pushed to DB.
            // Note: for positions x+ points to the right on the map and y+ points up.
            position: [0,0,0], // Position estimate [x,y, heading]
            uncertainty: [0,0,0], // Uncertainty in the measurement [+-x, +-y, +-angle]
            // These fields are only used if this packet corresponds to / is produced
            // from an image. commandLookupID is 0 if this data did not come while any
            // commands were being executed.
            commandLookupID: 1, // ID of Command which put the rover in this state
            // Moves longer than frame.len will get broken up into multiple smaller
            // moves for a breadcrumb image to be taken after each one.
            // Note: n and tot count from 1. n is 0 if this data does not correspond
            // with any image-bearing point (ie. end of a command or a breadcrumb frame)
            frame: {
                len: 0, // [cm] Length of segmenting (rover didn't move any distance between frames (special case here for the deploy command since it's a non-move command which generates an image)
                n: 1, // Frame number corresponding to this data point (frame 1/1)
                tot: 1 // Total number of frames in this move (only 1 image gets taken)
            }
        }),

        // At t=142s, after evaluating the scene and determining safety, Command 2 MoveForward(30cm, 2cm/s) 
        // is sent to the rover (note: this will take 15s to execute once the rover starts moving).
        // As soon as the rover receives this command and starts moving at t=150s, accelerometer and 
        // odometry data start getting sent back (let's say every 2s), which are processed by localization 
        // and cause the following data packets to get pushed to the collection.
    
        // deadreckoning update - collected 2s into rover movement after first packet is sent
        new LocalizationData({
            lookupID: 2,
            collectionTime: 152, // time data was collected (on rover)
            sendTime: 160, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.3, 4.1, 0.5], // cm, cm, deg
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
            position: [-0.2, 7.9, 1], // cm, cm, deg
            uncertainty: [0.3, 1.5, 0.5], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // NOTE: At t=155s, after moving 10cm (5s) into the 30cm move, the first frame (a breadcrumb image) is 
        // taken and will be used to create a better rover position estimate by the localizer. 
        // However, because it takes 21s for images to be transmitted, this update won't be pushed to the DB 
        // until t=176s (assuming <1s of processing time). HOWEVER, the move will be over by that point; so, 
        // for the sake of creating an interesting test case where a frame image comes in while the rover is 
        // still moving, we'll pretend this images arrives early at t=172s.

        // Meanwhile, telemetry data keeps streaming in, producing localization updates:

        // deadreckoning update - collected 6s into rover movement 
        new LocalizationData({
            lookupID: 4,
            collectionTime: 156, // time data was collected (on rover)
            sendTime: 164, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [-0.2, 12.1, 2], // cm, cm, deg
            uncertainty: [0.4, 2.0, 0.8], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 8s into rover movement 
        new LocalizationData({
            lookupID: 5,
            collectionTime: 158, // time data was collected (on rover)
            sendTime: 166, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [1, 15.8, 3], // cm, cm, deg
            uncertainty: [0.5, 2.5, 1.0], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 10s into rover movement 
        new LocalizationData({
            lookupID: 6,
            collectionTime: 160, // time data was collected (on rover)
            sendTime: 168, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [1.3, 20.1, 4], // cm, cm, deg
            uncertainty: [0.6, 3.0, 1.3], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // NOTE: At t=160s, after moving 20cm (10s) into the 30cm move, the second frame (a breadcrumb image) 
        // is taken and will be used to create a better rover position estimate by the localizer. 
        // However, because it takes 21s for images to be transmitted, this update won't be pushed to the 
        // DB until t=181s (assuming <1s of processing time).

        // Meanwhile, telemetry data keeps streaming in, producing localization updates:

        // deadreckoning update - collected 12s into rover movement 
        new LocalizationData({
            lookupID: 7,
            collectionTime: 162, // time data was collected (on rover)
            sendTime: 170, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [2.0, 24.8, 6], // cm, cm, deg
            uncertainty: [0.7, 3.5, 1.5], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 14s into rover movement 
        new LocalizationData({
            lookupID: 8,
            collectionTime: 164, // time data was collected (on rover)
            sendTime: 172, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [1.8, 28.9, 5.6], // cm, cm, deg
            uncertainty: [0.8, 4.0, 1.8], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // NOTE: At t=172s, the breadcrumb image from frame 1 finishes being transmitted and processed and is pushed 
        // to the database.

        // image-based update - collected 5s into rover movement   
        new LocalizationData({
            lookupID: 9,
            collectionTime: 155, // time data was collected (on rover)
            sendTime: 172, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.1, 10.0, 0.2], // cm, cm, deg
            uncertainty: [0.1, 0.1, 0.2], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                len: 10, // [cm] Length of segments
                n: 1, // Frame number corresponding to this data point (frame 1/3)
                tot: 3 // Total number of frames in this move
            }
        }),

        // BIG NOTE: When the localizer finishes processing the image above, it will also go back and 
        // reintegrate all deadreckoning data that was collected after the image (later collectionTime), 
        // updating the associated database entries to provide a better position estimate. This shouldn't 
        // impact the frontend, since the lazy list entries will automatically (reactively) get updated too 
        // (same way the command line state entries get updated).

        // This will cause the following changes (namely, to position, uncertainty, and sendTime):

        // deadreckoning update - collected 6s into rover movement   
        new LocalizationData({
            lookupID: 4,
            collectionTime: 156, // time data was collected (on rover)
            sendTime: 172, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.1, 12.0, 0.3], // cm, cm, deg
            uncertainty: [0.1, 0.5, 0.3], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 8s into rover movement   
        new LocalizationData({
            lookupID: 5,
            collectionTime: 158, // time data was collected (on rover)
            sendTime: 172, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.2, 16.1, 0.4], // cm, cm, deg
            uncertainty: [0.2, 1.0, 0.5], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 10s into rover movement  
        new LocalizationData({
            lookupID: 6,
            collectionTime: 160, // time data was collected (on rover)
            sendTime: 172, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.4, 20.2, 0.1], // cm, cm, deg
            uncertainty: [0.3, 1.5, 0.8], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 12s into rover movement  
        new LocalizationData({
            lookupID: 7,
            collectionTime: 162, // time data was collected (on rover)
            sendTime: 172, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.2, 24.4, -0.3], // cm, cm, deg
            uncertainty: [0.4, 2.0, 1.0], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 14s into rover movement  
        new LocalizationData({
            lookupID: 8,
            collectionTime: 164, // time data was collected (on rover)
            sendTime: 172, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.5, 28.6, -0.8], // cm, cm, deg
            uncertainty: [0.5, 2.5, 1.3], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // Telemetry data still comes in. In this case, there happens to be only one more telemetry packet.
        // The final telemetry packet will come in 8s after the rover stops moving at t=165s:

        // deadreckoning update - collected 15s into rover movement  
        new LocalizationData({
            lookupID: 10,
            collectionTime: 165, // time data was collected (on rover)
            sendTime: 173, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.8, 31.2, -1.1], // cm, cm, deg
            uncertainty: [0.6, 3.0, 1.5], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // NOTE: At t=165s, after moving 30cm (15s) into the 30cm move (the end), the final frame (a mission image) 
        // is taken and will be used to create a better rover position estimate by the localizer. However, 
        // because it takes 21s for images to be transmitted, this update won't be pushed to the DB until t=186s 
        // (assuming <1s of processing time).
    
        // NOTE: At t=181s, the breadcrumb image from frame 2 finishes being transmitted and processed and is 
        // pushed to the database.

        // image-based update - collected 10s into rover movement  
        new LocalizationData({
            lookupID: 11,
            collectionTime: 160, // time data was collected (on rover)
            sendTime: 181, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.2, 20.1, 0.2], // cm, cm, deg
            uncertainty: [0.1, 0.1, 0.2], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                len: 10, // [cm] Length of segments
                n: 2, // Frame number corresponding to this data point (frame 2 of 3)
                tot: 3 // Total number of frames in this move
            }
        }),

        // BIG NOTE: When the localizer finishes processing the image above, it will also go back and 
        // reintegrate all deadreckoning data that was collected after the image (later collectionTime), 
        // updating the associated database entries to provide a better position estimate. This shouldn't 
        // impact the frontend, since the lazy list entries will automatically (reactively) get updated too 
        // (same way the command line state entries get updated).

        // This will cause the following changes (namely, to position, uncertainty, and sendTime):

        // deadreckoning update - collected 10s into rover movement
        new LocalizationData({
            lookupID: 6,
            collectionTime: 160, // time data was collected (on rover)
            sendTime: 181, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.2, 20.1, 0.2], // cm, cm, deg
            uncertainty: [0.1, 0.1, 0.2], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 12s into rover movement
        new LocalizationData({
            lookupID: 7,
            collectionTime: 162, // time data was collected (on rover)
            sendTime: 181, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [0.1, 24.2, 0.1], // cm, cm, deg
            uncertainty: [0.2, 0.5, 0.3], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 14s into rover movement
        new LocalizationData({
            lookupID: 8,
            collectionTime: 164, // time data was collected (on rover)
            sendTime: 181, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [-0.1, 28.5, -0.1], // cm, cm, deg
            uncertainty: [0.3, 1.0, 0.5], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // deadreckoning update - collected 15s into rover movement
        new LocalizationData({
            lookupID: 10,
            collectionTime: 165, // time data was collected (on rover)
            sendTime: 181, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [-0.2, 30.6, -0.3], // cm, cm, deg
            uncertainty: [0.4, 1.5, 0.8], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                n: 0, // this data isn't associated with an image frame
            }
        }),

        // NOTE: At t=186s, the mission image from the final frame (3) finishes being transmitted and processed 
        // and is pushed to the database.

        new LocalizationData({
            lookupID: 12,
            collectionTime: 165, // time data was collected (on rover)
            sendTime: 186, // time data packet was finished being processed by localization and sent from backend to frontend
            position: [-0.2, 30.2, 0.3], // cm, cm, deg
            uncertainty: [0.1, 0.1, 0.2], // cm, cm, deg
            commandLookupID: 3, // still associated with the same command
            frame: {
                len: 10, // [cm] Length of segments
                n: 3, // Frame number corresponding to this data point (frame 3 of 3)
                tot: 3 // Total number of frames in this move
            }
        })
    ]
];

export default data;