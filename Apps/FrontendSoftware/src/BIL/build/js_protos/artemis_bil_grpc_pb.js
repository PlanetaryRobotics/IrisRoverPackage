// GENERATED CODE -- DO NOT EDIT!

// Original file comments:
// Prototype proto for the BIL used by ARTEMIS (Python-Unity).
'use strict';
var grpc = require('@grpc/grpc-js');
var artemis_bil_pb = require('./artemis_bil_pb.js');
var db_pb = require('./db_pb.js');

function serialize_iris_artemis_bil_SendTelemAck(arg) {
  if (!(arg instanceof artemis_bil_pb.SendTelemAck)) {
    throw new Error('Expected argument of type iris_artemis_bil.SendTelemAck');
  }
  return Buffer.from(arg.serializeBinary());
}

function deserialize_iris_artemis_bil_SendTelemAck(buffer_arg) {
  return artemis_bil_pb.SendTelemAck.deserializeBinary(new Uint8Array(buffer_arg));
}

function serialize_iris_bil_TelemItem(arg) {
  if (!(arg instanceof db_pb.TelemItem)) {
    throw new Error('Expected argument of type iris_bil.TelemItem');
  }
  return Buffer.from(arg.serializeBinary());
}

function deserialize_iris_bil_TelemItem(buffer_arg) {
  return db_pb.TelemItem.deserializeBinary(new Uint8Array(buffer_arg));
}


// Service running in the BIL (Python).
// In the prototype, it basically just handles data send requests from the
// client (Unity). In the future, this will also handle streaming uplink data
// into Unity.
var ArtemisBilService = exports.ArtemisBilService = {
  // Asks the BIL to send telem to the rest of the stack:
sendTelem: {
    path: '/iris_artemis_bil.ArtemisBil/SendTelem',
    requestStream: false,
    responseStream: false,
    requestType: db_pb.TelemItem,
    responseType: artemis_bil_pb.SendTelemAck,
    requestSerialize: serialize_iris_bil_TelemItem,
    requestDeserialize: deserialize_iris_bil_TelemItem,
    responseSerialize: serialize_iris_artemis_bil_SendTelemAck,
    responseDeserialize: deserialize_iris_artemis_bil_SendTelemAck,
  },
};

exports.ArtemisBilClient = grpc.makeGenericClientConstructor(ArtemisBilService);
