# Copyright 2020 The IREE Authors
#
# Licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

load("//build_tools/bazel:iree_check_test.bzl", "iree_check_test_suite")
load("//build_tools/bazel:iree_lit_test.bzl", "iree_lit_test_suite")
load("//build_tools/bazel:enforce_glob.bzl", "enforce_glob")

package(
    default_visibility = ["//visibility:public"],
    features = ["layering_check"],
    licenses = ["notice"],  # Apache 2.0
)

iree_lit_test_suite(
    name = "lit",
    srcs = enforce_glob(
        [
            "failure.mlir",
            "success.mlir",
            "unavailable.mlir",
        ],
        include = ["*.mlir"],
    ),
    cfg = "//runtime:lit.cfg.py",
    tags = ["hostonly"],
    tools = [
        "//tools:iree-check-module",
        "//tools:iree-compile",
        "//tools:iree-run-module",
        "@llvm-project//llvm:FileCheck",
    ],
)

iree_check_test_suite(
    name = "check",
    srcs = ["success.mlir"],
    compiler_flags = ["--iree-input-type=mhlo"],
)

iree_check_test_suite(
    name = "check_failure",
    srcs = ["failure.mlir"],
    compiler_flags = ["--iree-input-type=mhlo"],
    runner_args = ["--expect_failure"],
)
