# Copyright 2022 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

load("@llvm-project//mlir:tblgen.bzl", "td_library")
load("//build_tools/bazel:build_defs.oss.bzl", "iree_compiler_cc_library")
load("//build_tools/bazel:enforce_glob.bzl", "enforce_glob")
load("//build_tools/bazel:iree_tablegen.bzl", "iree_gentbl_cc_library")

package(
    default_visibility = ["//visibility:public"],
    features = ["layering_check"],
    licenses = ["notice"],  # Apache 2.0
)

td_library(
    name = "td_files",
    srcs = enforce_glob(
        [
            "LLVMGPUExtensionsOps.td",
        ],
        include = ["*.td"],
    ),
    deps = [
        "@llvm-project//mlir:InferTypeOpInterfaceTdFiles",
        "@llvm-project//mlir:OpBaseTdFiles",
        "@llvm-project//mlir:TransformDialectTdFiles",
    ],
)

iree_gentbl_cc_library(
    name = "LLVMGPUExtensionsOpGen",
    tbl_outs = [
        (
            ["--gen-op-decls"],
            "LLVMGPUExtensionsOps.h.inc",
        ),
        (
            ["--gen-op-defs"],
            "LLVMGPUExtensionsOps.cpp.inc",
        ),
    ],
    tblgen = "@llvm-project//mlir:mlir-tblgen",
    td_file = "LLVMGPUExtensionsOps.td",
    deps = [":td_files"],
)

iree_compiler_cc_library(
    name = "LLVMGPUExtensions",
    srcs = [
        "LLVMGPUExtensions.cpp",
        "LLVMGPUExtensionsOps.cpp.inc",
    ],
    hdrs = [
        "LLVMGPUExtensions.h",
        "LLVMGPUExtensionsOps.h.inc",
    ],
    deps = [
        ":LLVMGPUExtensionsOpGen",
        "//compiler/src/iree/compiler/Dialect/HAL/IR",
        "//llvm-external-projects/iree-dialects:IREEDialectsTransforms",
        "//llvm-external-projects/iree-dialects:IREELinalgTransformDialect",
        "@llvm-project//mlir:ArithmeticDialect",
        "@llvm-project//mlir:FuncDialect",
        "@llvm-project//mlir:GPUDialect",
        "@llvm-project//mlir:IR",
        "@llvm-project//mlir:MemRefDialect",
        "@llvm-project//mlir:PDLDialect",
        "@llvm-project//mlir:Pass",
        "@llvm-project//mlir:SCFDialect",
        "@llvm-project//mlir:TransformDialect",
        "@llvm-project//mlir:Transforms",
        "@llvm-project//mlir:VectorDialect",
        "@llvm-project//mlir:VectorTransforms",
    ],
)
