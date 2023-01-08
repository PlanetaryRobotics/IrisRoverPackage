// GENERATED CODE -- DO NOT EDIT!

'use strict';
var grpc = require('@grpc/grpc-js');
var bil_pb = require('./bil_pb.js');
var time_pb = require('./time_pb.js');
var db_pb = require('./db_pb.js');

function serialize_iris_bil_BilDataRequest(arg) {
  if (!(arg instanceof bil_pb.BilDataRequest)) {
    throw new Error('Expected argument of type iris_bil.BilDataRequest');
  }
  return Buffer.from(arg.serializeBinary());
}

function deserialize_iris_bil_BilDataRequest(buffer_arg) {
  return bil_pb.BilDataRequest.deserializeBinary(new Uint8Array(buffer_arg));
}

function serialize_iris_bil_BilDataRequestResponse(arg) {
  if (!(arg instanceof bil_pb.BilDataRequestResponse)) {
    throw new Error('Expected argument of type iris_bil.BilDataRequestResponse');
  }
  return Buffer.from(arg.serializeBinary());
}

function deserialize_iris_bil_BilDataRequestResponse(buffer_arg) {
  return bil_pb.BilDataRequestResponse.deserializeBinary(new Uint8Array(buffer_arg));
}

function serialize_iris_bil_Command(arg) {
  if (!(arg instanceof db_pb.Command)) {
    throw new Error('Expected argument of type iris_bil.Command');
  }
  return Buffer.from(arg.serializeBinary());
}

function deserialize_iris_bil_Command(buffer_arg) {
  return db_pb.Command.deserializeBinary(new Uint8Array(buffer_arg));
}

function serialize_iris_bil_InternalCommandAck(arg) {
  if (!(arg instanceof bil_pb.InternalCommandAck)) {
    throw new Error('Expected argument of type iris_bil.InternalCommandAck');
  }
  return Buffer.from(arg.serializeBinary());
}

function deserialize_iris_bil_InternalCommandAck(buffer_arg) {
  return bil_pb.InternalCommandAck.deserializeBinary(new Uint8Array(buffer_arg));
}

function serialize_iris_bil_PingMessage(arg) {
  if (!(arg instanceof bil_pb.PingMessage)) {
    throw new Error('Expected argument of type iris_bil.PingMessage');
  }
  return Buffer.from(arg.serializeBinary());
}

function deserialize_iris_bil_PingMessage(buffer_arg) {
  return bil_pb.PingMessage.deserializeBinary(new Uint8Array(buffer_arg));
}


// Backend Interface Layer Service Definition:
var BilService = exports.BilService = {
  // Aliveness Ping (for debugging):
ping: {
    path: '/iris_bil.Bil/Ping',
    requestStream: false,
    responseStream: false,
    requestType: bil_pb.PingMessage,
    responseType: bil_pb.PingMessage,
    requestSerialize: serialize_iris_bil_PingMessage,
    requestDeserialize: deserialize_iris_bil_PingMessage,
    responseSerialize: serialize_iris_bil_PingMessage,
    responseDeserialize: deserialize_iris_bil_PingMessage,
  },
  // Tells the BIL to push the given command to the database:
sendCommand: {
    path: '/iris_bil.Bil/SendCommand',
    requestStream: false,
    responseStream: false,
    requestType: db_pb.Command,
    responseType: bil_pb.InternalCommandAck,
    requestSerialize: serialize_iris_bil_Command,
    requestDeserialize: deserialize_iris_bil_Command,
    responseSerialize: serialize_iris_bil_InternalCommandAck,
    responseDeserialize: deserialize_iris_bil_InternalCommandAck,
  },
  // Generic one-time request for stored data
// (generic because any received data goes straight into an event emitter):
requestStoredData: {
    path: '/iris_bil.Bil/RequestStoredData',
    requestStream: false,
    responseStream: false,
    requestType: bil_pb.BilDataRequest,
    responseType: bil_pb.BilDataRequestResponse,
    requestSerialize: serialize_iris_bil_BilDataRequest,
    requestDeserialize: deserialize_iris_bil_BilDataRequest,
    responseSerialize: serialize_iris_bil_BilDataRequestResponse,
    responseDeserialize: deserialize_iris_bil_BilDataRequestResponse,
  },
  // Stream all new data that matches the given request:
streamNewData: {
    path: '/iris_bil.Bil/StreamNewData',
    requestStream: false,
    responseStream: true,
    requestType: bil_pb.BilDataRequest,
    responseType: bil_pb.BilDataRequestResponse,
    requestSerialize: serialize_iris_bil_BilDataRequest,
    requestDeserialize: deserialize_iris_bil_BilDataRequest,
    responseSerialize: serialize_iris_bil_BilDataRequestResponse,
    responseDeserialize: deserialize_iris_bil_BilDataRequestResponse,
  },
};

exports.BilClient = grpc.makeGenericClientConstructor(BilService);
