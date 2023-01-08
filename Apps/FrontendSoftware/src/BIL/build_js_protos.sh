# Builds the BIL protobuf into static NodeJS:
IRIS_BIL_PROTOS_DIR="./protos_src"
IRIS_BIL_PROTOS_JS_OUT_DIR="./build/js_protos"

IRIS_BIL_PROTO_MAIN="$IRIS_BIL_PROTOS_DIR/bil.proto"
IRIS_BIL_PROTO_TIME_DEF="$IRIS_BIL_PROTOS_DIR/time.proto"
IRIS_BIL_PROTO_DB_DEF="$IRIS_BIL_PROTOS_DIR/db.proto"

mkdir -p $IRIS_BIL_PROTOS_JS_OUT_DIR

build_js_proto() {
    grpc_tools_node_protoc \
        -I$IRIS_BIL_PROTOS_DIR \
        --proto_path built_protos=$IRIS_BIL_PROTOS_DIR \
        --js_out=import_style=commonjs,binary:$IRIS_BIL_PROTOS_JS_OUT_DIR \
        --grpc_out=grpc_js:$IRIS_BIL_PROTOS_JS_OUT_DIR \
        $1
}

build_js_proto $IRIS_BIL_PROTO_TIME_DEF
build_js_proto $IRIS_BIL_PROTO_DB_DEF
build_js_proto $IRIS_BIL_PROTO_MAIN