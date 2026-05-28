#!/bin/sh

set -e

cmake -S . -B build_native -DBUILD_NATIVE=TRUE
cmake --build build_native
ctest --test-dir build_native --output-on-failure
