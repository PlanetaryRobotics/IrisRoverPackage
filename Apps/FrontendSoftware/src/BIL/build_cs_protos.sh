# Builds C-Sharp (C#) Protos for use in ARTEMIS (NOTE: this is a separate BIL
# service and is not part of the standard build-process for the Frontend and so
# is not included in `make build-bil`).
# To compile this, you'll need to make sure `PLATFORM` is set correctly below.
# This has only been tested compiling on Windows.
# You may also need to manually install protoc (`brew install protobuf for
# MacOS or https://github.com/protocolbuffers/protobuf/releases/ for Windows)
# for this one.
IRIS_BIL_PROTOS_DIR="./protos_src"
IRIS_BIL_PROTOS_CS_OUT_DIR="./build/cs_protos"

# PICK ONE:
# PLATFORM="macosx_x64"
PLATFORM="windows_x64"

mkdir -p $IRIS_BIL_PROTOS_CS_OUT_DIR

get_abs_filename() {
  # $1 : relative filename
  echo "$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
}

CSHARP_PLUGIN="./plugins/csharp/$PLATFORM/grpc_csharp_plugin"
CSHARP_PLUGIN_ABS_PATH=$(get_abs_filename $CSHARP_PLUGIN)

protoc \
    -I$IRIS_BIL_PROTOS_DIR \
    --csharp_out=$IRIS_BIL_PROTOS_CS_OUT_DIR \
    --grpc_out=$IRIS_BIL_PROTOS_CS_OUT_DIR \
    --plugin=protoc-gen-grpc=$CSHARP_PLUGIN_ABS_PATH \
    $(find $IRIS_BIL_PROTOS_DIR -name '*.proto')