# Builds the BIL protobuf into static NodeJS:
IRIS_BIL_PROTOS_DIR="./protos_src"
IRIS_BIL_PROTOS_JS_OUT_DIR="./build/js_protos"

mkdir -p $IRIS_BIL_PROTOS_JS_OUT_DIR

grpc_tools_node_protoc \
    -I$IRIS_BIL_PROTOS_DIR \
    --proto_path built_protos=$IRIS_BIL_PROTOS_DIR \
    --js_out=import_style=commonjs,binary:$IRIS_BIL_PROTOS_JS_OUT_DIR \
    --grpc_out=grpc_js:$IRIS_BIL_PROTOS_JS_OUT_DIR \
    $(find $IRIS_BIL_PROTOS_DIR -name '*.proto')