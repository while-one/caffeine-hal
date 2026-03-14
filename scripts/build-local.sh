#!/bin/bash
TARGET_STAGE=${1:-native}
CMAKE_TARGET=${2:-all} # New optional argument for CMake target

IMAGE="build-$TARGET_STAGE"

# Build the specific stage if it doesn't exist locally
docker build --target build-$TARGET_STAGE -t $IMAGE .

# Run build in container, forcing offline resolution for dependencies like GTest
docker run --rm -v "$(pwd)":/work -w /work $IMAGE \
    bash -c "rm -rf build && cmake -B build -DFETCHCONTENT_FULLY_DISCONNECTED=ON && cmake --build build --target $CMAKE_TARGET"
