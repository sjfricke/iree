# Copyright 2021 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

load("//build_tools/bazel:build_defs.oss.bzl", "iree_compiler_cc_library")

package(
    default_visibility = ["//visibility:public"],
    features = ["layering_check"],
    licenses = ["notice"],  # Apache 2.0
)

iree_compiler_cc_library(
    name = "UtilToVM",
    srcs = [
        "ConvertAlignmentOps.cpp",
        "ConvertGlobalOps.cpp",
        "ConvertListOps.cpp",
        "ConvertStatusOps.cpp",
        "ConvertUtilToVM.cpp",
    ],
    hdrs = [
        "ConvertUtilToVM.h",
    ],
    deps = [
        "//compiler/src/iree/compiler/Dialect/Util/IR",
        "//compiler/src/iree/compiler/Dialect/VM/Conversion",
        "//compiler/src/iree/compiler/Dialect/VM/IR",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:Pass",
        "@llvm-project//mlir:TransformUtils",
        "@llvm-project//mlir:Transforms",
    ],
)
