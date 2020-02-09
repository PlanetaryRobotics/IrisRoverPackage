import path from 'path'
import fs from 'fs'

import POICard from "@/data_classes/POICard.js";
import ImageData from '@/data_classes/ImageData.js'
import Tag from '@/data_classes/Tag.js'

// Simulate the binary image data which will be Received from the DB:
function bytes(file){
  let data = fs.readFileSync(file);
  return Uint8Array.from(data);
}

export default {
  state: {
    POIList: [
      new POICard(
        {
          importanceLevel: 1,
          category: "OBSTACLE",
          thumbnail: new ImageData({
            lookupID: 1,
            commandLookupID: 1,
            name: 'CR_M01_0000',
            bin: bytes(path.join(__static,'./images/NFwvadh.jpg')),
            camera: "Front",
            sendTime: new Date(),
            command: "forward",
            commandDesc: "10cm",
            tags: ["Nathan"]
          }),
          width: 5,
          height: 5,
          sizeUnit: "CM",
          tagList: [
            new Tag({name: "Dwayne", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Squid", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rock", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rocco", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Steve", initialImage:"CR_M01_0000", tagNames:[]}),
            new Tag({name: "Dwayne", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Squid", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rock", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rocco", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Steve", initialImage:"CR_M01_0000", tagNames:[]})
          ],
          creator: "Caitlin Coyiuto",
          description: "I am a hella beautiful rock on the moon.",
          images: [
            new ImageData({
              lookupID: 6,
              commandLookupID: 3,
              name: 'CR_M01_0005',
              bin: bytes(path.join(__static,'./images/466fWFX.jpg')),
              camera: "Rear",
              sendTime: new Date(),
              command: "forward",
              commandDesc: "10cm",
              tags: []
            }),
            new ImageData({
              lookupID: 7,
              commandLookupID: 4,
              name: 'CR_M01_0006',
              bin: bytes(path.join(__static,'./images/FBig7ow.jpg')),
              camera: "Front",
              sendTime: new Date(),
              command: "forward",
              commandDesc: "10cm",
              tags: []
            }),
            new ImageData({
              lookupID: 8,
              commandLookupID: 4,
              name: 'CR_M01_0007',
              bin: bytes(path.join(__static,'./images/MTipXK1.jpg')),
              camera: "Rear",
              sendTime: new Date(),
              command: "forward",
              commandDesc: "10cm",
              tags: []
            }),
            new ImageData({
              lookupID: 9,
              commandLookupID: 5,
              name: 'CR_M01_0008',
              bin: bytes(path.join(__static,'./images/dICMJTn.jpg')),
              camera: "Front",
              sendTime: new Date(),
              command: "forward",
              commandDesc: "10cm",
              tags: []
            }),
            new ImageData({
              lookupID: 10,
              commandLookupID: 5,
              name: 'CR_M01_0009',
              bin: bytes(path.join(__static,'./images/hOhhfSy.jpg')),
              camera: "Rear",
              sendTime: new Date(),
              command: "forward",
              commandDesc: "10cm",
              tags: []
            }),
          ]
        }
      ),
      new POICard(
        {
          importanceLevel: 2,
          category: "SHADOW",
          thumbnail: new ImageData({
            lookupID: 1,
            commandLookupID: 1,
            name: 'CR_M01_0000',
            bin: bytes(path.join(__static,'./images/NFwvadh.jpg')),
            camera: "Front",
            sendTime: new Date(),
            command: "forward",
            commandDesc: "10cm",
            tags: ["Nathan"]
          }),
          width: 5,
          height: 5,
          sizeUnit: "CM",
          tagList: [
            new Tag({name: "Dwayne", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Squid", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rock", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rocco", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Steve", initialImage:"CR_M01_0000", tagNames:[]}),
            new Tag({name: "Dwayne", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Squid", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rock", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rocco", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Steve", initialImage:"CR_M01_0000", tagNames:[]})
          ],
          creator: "Caitlin Coyiuto",
          description: "I am a hella beautiful rock on the moon."
        }
      ),
      new POICard(
        {
          importanceLevel: 3,
          category: "ATTRACTION",
          thumbnail: new ImageData({
            lookupID: 1,
            commandLookupID: 1,
            name: 'CR_M01_0000',
            bin: bytes(path.join(__static,'./images/NFwvadh.jpg')),
            camera: "Front",
            sendTime: new Date(),
            command: "forward",
            commandDesc: "10cm",
            tags: ["Nathan"]
          }),
          width: 5,
          height: 5,
          sizeUnit: "CM",
          tagList: [
            new Tag({name: "Dwayne", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Squid", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rock", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rocco", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Steve", initialImage:"CR_M01_0000", tagNames:[]}),
            new Tag({name: "Dwayne", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Squid", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rock", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Rocco", initialImage:"CR_M01_0001", tagNames:[]}),
            new Tag({name: "Steve", initialImage:"CR_M01_0000", tagNames:[]})
          ],
          creator: "Caitlin Coyiuto",
          description: "I am a hella beautiful rock on the moon."
        }
      )
    ]
  },
  getters: {
    POIList: state => {
      return state.POIList;
    }
  },
  mutations: {
    
  }
};
