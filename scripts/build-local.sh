#!/bin/bash
TARGET=${1:-native}
IMAGE="build-$TARGET"

# Build the specific stage if it doesn't exist locally
docker build --target build-$TARGET -t $IMAGE .

# Run build in container, forcing offline resolution for dependencies like GTest
docker run --rm -v $(pwd):/work -w /work $IMAGE \
    bash -c "cmake -B build -DFETCHCONTENT_FULLY_DISCONNECTED=ON && cmake --build build"
