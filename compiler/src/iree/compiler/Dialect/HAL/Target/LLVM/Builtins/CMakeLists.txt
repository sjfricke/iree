################################################################################
# Autogenerated by build_tools/bazel_to_cmake/bazel_to_cmake.py from           #
# compiler/src/iree/compiler/Dialect/HAL/Target/LLVM/Builtins/BUILD            #
#                                                                              #
# Use iree_cmake_extra_content from iree/build_defs.oss.bzl to add arbitrary   #
# CMake-only content.                                                          #
#                                                                              #
# To disable autogeneration for this file entirely, delete this header.        #
################################################################################

iree_add_all_subdirs()

iree_cc_library(
  NAME
    Builtins
  HDRS
    "Device.h"
    "Musl.h"
  SRCS
    "Device.cpp"
    "Musl.cpp"
  DEPS
    LLVMBitReader
    LLVMCore
    LLVMSupport
    LLVMTarget
    MLIRSupport
    iree::builtins::device::bin::libdevice
    iree::builtins::musl::bin::libmusl
  PUBLIC
)

### BAZEL_TO_CMAKE_PRESERVES_ALL_CONTENT_BELOW_THIS_LINE ###
