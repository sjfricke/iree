#!/bin/bash
# Copyright 2022 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Imports models and generates compiled VMFB modules. The first argument should
# point to a IREE installation directory (contains IREE tools). Default points
# to "build-host/install".

set -xeuo pipefail

# Print the UTC time when set -x is on.
export PS4='[$(date -u "+%T %Z")] '

# Check these exist and print the versions for later debugging.
CMAKE_BIN=${CMAKE_BIN:-$(which cmake)}
"${CMAKE_BIN}" --version
"${CC}" --version
"${CXX}" --version
ninja --version
python3 --version

echo "Initializing submodules"
git submodule sync
git submodule update --init --jobs 8 --depth 1

ROOT_DIR=$(git rev-parse --show-toplevel)
# Get the root of host binaries, or default to "${ROOT_DIR}/build-host/install".
HOST_BINARY_ROOT="$(realpath ${1:-${ROOT_DIR}/build-host/install})"

cd "${ROOT_DIR}"

# BUILD the iree-import-* binaries for importing models to benchmark.
cd "${ROOT_DIR}/integrations/tensorflow"
BAZEL_CMD=(bazel --noworkspace_rc --bazelrc=build_tools/bazel/iree-tf.bazelrc)
BAZEL_BINDIR="$(${BAZEL_CMD[@]} info bazel-bin)"
"${BAZEL_CMD[@]}" build \
      //iree_tf_compiler:iree-import-tflite \
      //iree_tf_compiler:iree-import-tf \
      --config=generic_clang \
      --config=remote_cache_bazel_ci
# So the benchmark build below can find the importer binaries that were built.
export PATH="$PWD/bazel-bin/iree_tf_compiler:$PATH"

# --------------------------------------------------------------------------- #
# Build for the target (linux-x86_64).

cd "${ROOT_DIR}"

if [ -d "build-targets/linux-x86_64" ]
then
  echo "linux-x86_64 directory already exists. Will use cached results there."
else
  echo "linux-x86_64 directory does not already exist. Creating a new one."
  mkdir -p build-targets/linux-x86_64
fi
cd build-targets/linux-x86_64

"${CMAKE_BIN}" -G Ninja ../.. \
  -DIREE_HOST_BINARY_ROOT="${HOST_BINARY_ROOT}" \
  -DIREE_BUILD_COMPILER=OFF \
  -DIREE_BUILD_TESTS=OFF \
  -DIREE_BUILD_SAMPLES=OFF \
  -DIREE_BUILD_BENCHMARKS=ON \
  -DIREE_ENABLE_COMPILATION_BENCHMARKS=ON

"${CMAKE_BIN}" --build . --target iree-benchmark-import-models -- -k 0
"${CMAKE_BIN}" --build . --target iree-benchmark-suites-linux-x86_64 -- -k 0
# --------------------------------------------------------------------------- #

# --------------------------------------------------------------------------- #
# Build for the target (linux-riscv).

cd "${ROOT_DIR}"

if [ -d "build-targets/linux-riscv" ]
then
  echo "linux-riscv directory already exists. Will use cached results there."
else
  echo "linux-riscv directory does not already exist. Creating a new one."
  mkdir -p build-targets/linux-riscv
fi
cd build-targets/linux-riscv

"${CMAKE_BIN}" -G Ninja ../.. \
  -DIREE_HOST_BINARY_ROOT="${HOST_BINARY_ROOT}" \
  -DIREE_BUILD_COMPILER=OFF \
  -DIREE_BUILD_TESTS=OFF \
  -DIREE_BUILD_SAMPLES=OFF \
  -DIREE_BUILD_BENCHMARKS=ON \
  -DIREE_ENABLE_COMPILATION_BENCHMARKS=ON

"${CMAKE_BIN}" --build . --target iree-benchmark-suites-linux-riscv -- -k 0
# --------------------------------------------------------------------------- #
