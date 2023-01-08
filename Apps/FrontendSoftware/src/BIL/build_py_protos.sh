# Builds the BIL protobuf into a python module:
IRIS_BIL_PROTOS_DIR="./protos_src"
PY_PACKAGE_NAME="built_protos"
IRIS_BIL_PROTOS_PY_OUT_DIR="./py_src/$PY_PACKAGE_NAME"

IRIS_BIL_PROTO_MAIN="$IRIS_BIL_PROTOS_DIR/bil.proto"
IRIS_BIL_PROTO_TIME_DEF="$IRIS_BIL_PROTOS_DIR/time.proto"
IRIS_BIL_PROTO_DB_DEF="$IRIS_BIL_PROTOS_DIR/db.proto"

mkdir -p $IRIS_BIL_PROTOS_PY_OUT_DIR

pyenv exec python3 -m grpc_tools.protoc \
    -I$IRIS_BIL_PROTOS_DIR \
    --proto_path $PY_PACKAGE_NAME=$IRIS_BIL_PROTOS_DIR \
    --grpc_python_out=$IRIS_BIL_PROTOS_PY_OUT_DIR \
    --python_out=$IRIS_BIL_PROTOS_PY_OUT_DIR \
    --pyi_out=$IRIS_BIL_PROTOS_PY_OUT_DIR \
    $(find $IRIS_BIL_PROTOS_DIR -name '*.proto')

# Protoc by default uses absolute imports which won't work when we're throwing
# them all in a package, so, in lieu of a cli option in `grpc_tools.protoc`,
# this script replaces all PB2 imports with relative imports:
find $IRIS_BIL_PROTOS_PY_OUT_DIR -maxdepth 1 -type f -exec pyenv exec python3 ./build_py_protos__fix_rel_imports.py {} \;

# Make sure the built directory contains an `__init__.py` file:
touch "$IRIS_BIL_PROTOS_PY_OUT_DIR/__init__.py"
