import path from 'path'
import fs from 'fs'

import POICard from "@/data_classes/POICard.js";
import POIList from "@/data_classes/POIList.js";
import ImageData from '@/data_classes/ImageData.js'
import Tag from '@/data_classes/Tag.js'

/* global __static */ // <- keep eslint from complaining about the __static directory
// Simulate the binary image data which will be Received from the DB:
function bytes(file){
  let data = fs.readFileSync(file);
  return Buffer.from(Uint8Array.from(data));
}

const TAGLIST = [
  new Tag({name: "rocco", initialImage:"CR_M01_0001", tagNames:[]}),
  new Tag({name: "ricco", initialImage:"CR_M01_0001", tagNames:[]}),
  new Tag({name: "darkrocco", initialImage:"CR_M01_0001", tagNames:[]}),
  new Tag({name: "moonrock", initialImage:"CR_M01_0001", tagNames:[]}),
  new Tag({name: "hole", initialImage:"CR_M01_0001", tagNames:[]}),
  new Tag({name: "steve", initialImage:"CR_M01_0000", tagNames:[]}),
  new Tag({name: "pebble", initialImage:"CR_M01_0000", tagNames:[]}),
  new Tag({name: "aaaaaa", initialImage:"CR_M01_0000", tagNames:[]}),
  new Tag({name: "ccccc", initialImage:"CR_M01_0000", tagNames:[]}),
  new Tag({name: "iamaverylongnameok", initialImage:"CR_M01_0000", tagNames:[]}),
];

var cards = [
  new POICard(
    {
      importanceLevel: 1,
      category: "OBSTACLE",
      thumbnail: new ImageData({
        name: 'CR_M01_0000',
        bin: bytes(path.join(__static,'./images/DhsYo0S.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: ["Nathan"]
      }),
      width: 20,
      height: 20,
      depth: 5,
      location: [30, 30],
      sizeUnit: "CM",
      tagList: [...TAGLIST.slice(0,2)],
      creator: "Caitlin Coyiuto",
      description: "Something's on this moon.",
      images: [
        new ImageData({
          name: 'CR_M01_0001',
          bin: bytes(path.join(__static,'./images/Kd2V45y.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "Forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0002',
          bin: bytes(path.join(__static,'./images/uJg1pom.jpg')),
          camera: "Rear",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0003',
          bin: bytes(path.join(__static,'./images/OUuN3i4.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0004',
          bin: bytes(path.join(__static,'./images/E9DbeBD.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0005',
          bin: bytes(path.join(__static,'./images/466fWFX.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0006',
          bin: bytes(path.join(__static,'./images/Kd2V45y.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "Forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0007',
          bin: bytes(path.join(__static,'./images/uJg1pom.jpg')),
          camera: "Rear",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0008',
          bin: bytes(path.join(__static,'./images/OUuN3i4.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0009',
          bin: bytes(path.join(__static,'./images/E9DbeBD.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0010',
          bin: bytes(path.join(__static,'./images/466fWFX.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0011',
          bin: bytes(path.join(__static,'./images/Kd2V45y.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "Forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0012',
          bin: bytes(path.join(__static,'./images/uJg1pom.jpg')),
          camera: "Rear",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0013',
          bin: bytes(path.join(__static,'./images/OUuN3i4.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0014',
          bin: bytes(path.join(__static,'./images/E9DbeBD.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0005',
          bin: bytes(path.join(__static,'./images/466fWFX.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0001',
          bin: bytes(path.join(__static,'./images/Kd2V45y.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "Forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0015',
          bin: bytes(path.join(__static,'./images/uJg1pom.jpg')),
          camera: "Rear",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0016',
          bin: bytes(path.join(__static,'./images/OUuN3i4.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0017',
          bin: bytes(path.join(__static,'./images/E9DbeBD.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0018',
          bin: bytes(path.join(__static,'./images/466fWFX.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
      ],
    }
  ),
  new POICard(
    {
      importanceLevel: 2,
      category: "SHADOW",
      thumbnail: new ImageData({
        name: 'CR_M01_0000',
        bin: bytes(path.join(__static,'./images/NFwvadh.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: ["Nathan"]
      }),
      width: 10,
      height: 10,
      depth: 5,
      location: [30, 90],
      sizeUnit: "CM",
      tagList: [...TAGLIST.slice(2,3)],
      creator: "Caitlin Coyiuto",
      description: "Lorem ipsum in a crater.",
      images: [
        new ImageData({
          name: 'CR_M01_0001',
          bin: bytes(path.join(__static,'./images/Kd2V45y.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "Forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0002',
          bin: bytes(path.join(__static,'./images/uJg1pom.jpg')),
          camera: "Rear",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0003',
          bin: bytes(path.join(__static,'./images/OUuN3i4.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
      ]
    }
  ),
  new POICard(
    {
      importanceLevel: 3,
      category: "ATTRACTION",
      thumbnail: new ImageData({
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
      depth: 5,
      location: [-30, 100],
      sizeUnit: "CM",
      tagList: [...TAGLIST.slice(3,4)],
      creator: "Caitlin Coyiuto",
      description: "How many ipsums can fit in a crater?",
      images: [
        new ImageData({
          name: 'CR_M01_0001',
          bin: bytes(path.join(__static,'./images/Kd2V45y.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "Forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0002',
          bin: bytes(path.join(__static,'./images/uJg1pom.jpg')),
          camera: "Rear",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0003',
          bin: bytes(path.join(__static,'./images/OUuN3i4.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
      ]
    }
  ),
  new POICard(
    {
      importanceLevel: 3,
      category: "ATTRACTION",
      thumbnail: new ImageData({
        name: 'CR_M01_0000',
        bin: bytes(path.join(__static,'./images/NFwvadh.jpg')),
        camera: "Front",
        sendTime: new Date(),
        command: "forward",
        commandDesc: "10cm",
        tags: ["Nathan"]
      }),
      width: 15,
      height: 15,
      depth: 5,
      location: [60, 60],
      sizeUnit: "CM",
      tagList: [...TAGLIST.slice(5,10)],
      creator: "Caitlin Coyiuto",
      description: "I am a hella beautiful rock on the moon.",
      images: [
        new ImageData({
          name: 'CR_M01_0001',
          bin: bytes(path.join(__static,'./images/Kd2V45y.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "Forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0002',
          bin: bytes(path.join(__static,'./images/uJg1pom.jpg')),
          camera: "Rear",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
        new ImageData({
          name: 'CR_M01_0003',
          bin: bytes(path.join(__static,'./images/OUuN3i4.jpg')),
          camera: "Front",
          sendTime: new Date(),
          command: "forward",
          commandDesc: "10cm",
          tags: [],
        }),
      ]
    }
  ),
  
]

export var testPOIList = new POIList(cards);


