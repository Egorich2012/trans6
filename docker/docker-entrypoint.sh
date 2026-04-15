#!/bin/bash
set -e

echo "=== BUILD START ==="

mkdir -p build
cd build

cmake ..
make -j$(nproc)

echo "=== BUILD DONE ==="

echo "=== RUN ==="

if [ "$1" = "test" ]; then
    ./main ../examples/input.txt
else
    ./main "$@"
fi