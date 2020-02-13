// import path from 'path'
// import fs from 'fs'

import POICard from "@/data_classes/POICard.js";
import ImageData from '@/data_classes/ImageData.js'
import Tag from '@/data_classes/Tag.js'

// Simulate the binary image data which will be Received from the DB:
// function bytes(file){
//   let data = fs.readFileSync(file);
//   return Uint8Array.from(data);
// }

export default {
  state: {
    POIList: [
      new POICard(
        {
          importanceLevel: 1,
          category: "OBSTACLE",
          thumbnail: new ImageData({
            name: 'CR_M01_0000',
            // bin: bytes(path.join(__static,'./images/NFwvadh.jpg')),
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
              name: 'CR_M01_0001',
              // bin: bytes(path.join(__static,'./images/Kd2V45y.jpg')),
              camera: "Front",
              sendTime: new Date(),
              command: "Forward",
              commandDesc: "10cm",
              tags: []
            }),
            new ImageData({
              name: 'CR_M01_0002',
              // bin: bytes(path.join(__static,'./images/uJg1pom.jpg')),
              camera: "Rear",
              sendTime: new Date(),
              command: "forward",
              commandDesc: "10cm",
              tags: []
            }),
            new ImageData({
              name: 'CR_M01_0003',
              // bin: bytes(path.join(__static,'./images/OUuN3i4.jpg')),
              camera: "Front",
              sendTime: new Date(),
              command: "forward",
              commandDesc: "10cm",
              tags: []
            }),
            new ImageData({
              name: 'CR_M01_0004',
              // bin: bytes(path.join(__static,'./images/E9DbeBD.jpg')),
              camera: "Front",
              sendTime: new Date(),
              command: "forward",
              commandDesc: "10cm",
              tags: []
            }),
            new ImageData({
              name: 'CR_M01_0005',
              // bin: bytes(path.join(__static,'./images/466fWFX.jpg')),
              camera: "Front",
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
            name: 'CR_M01_0000',
            // bin: bytes(path.join(__static,'./images/NFwvadh.jpg')),
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
            name: 'CR_M01_0000',
            // bin: bytes(path.join(__static,'./images/NFwvadh.jpg')),
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
