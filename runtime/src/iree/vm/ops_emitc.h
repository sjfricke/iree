// Copyright 2021 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_VM_OPS_EMITC_H_
#define IREE_VM_OPS_EMITC_H_

// This file contains utility macros used for things that EmitC  can't handle
// directly.

// Assign a value through a pointer variable
#define EMITC_DEREF_ASSIGN_VALUE(ptr, value) *(ptr) = (value)

// Assign a value pointed to by `ptr` through a pointer variable
#define EMITC_DEREF_ASSIGN_PTR(ptr, value) *(ptr) = *(value)

// Access a member of a struct
#define EMITC_STRUCT_MEMBER(struct, member) (struct).member

// Access the address of a member of a struct
#define EMITC_STRUCT_MEMBER_ADDRESS(struct, member) &(struct).member

// Assign a value to a member of a struct
#define EMITC_STRUCT_MEMBER_ASSIGN(struct, member, value) \
  (struct).member = (value)

// Access a member of a pointer to a struct
#define EMITC_STRUCT_PTR_MEMBER(struct, member) (struct)->member

// Call a function pointer of a pointer to a struct with the given arguments
#define EMITC_STRUCT_PTR_MEMBER_CALL(struct, member, ...) \
  (struct)->member(__VA_ARGS__)

// Access the address of a member of a pointer to a struct
#define EMITC_STRUCT_PTR_MEMBER_ADDRESS(struct, member) &(struct)->member

// Assign a value to a member of a pointer to a struct
#define EMITC_STRUCT_PTR_MEMBER_ASSIGN(struct, member, value) \
  (struct)->member = (value)

// Create a typdef struct
#define EMITC_TYPEDEF_STRUCT(typename, body) \
  typedef struct {                           \
    body                                     \
  } typename;

// Get the address of an array element
#define EMITC_ARRAY_ELEMENT_ADDRESS(array, index) &(array)[index]

// Unary operations
#define EMITC_NOT(arg) (!(arg))

// Binary operations
#define EMITC_AND(lhs, rhs) ((lhs) && (rhs))
#define EMITC_EQ(lhs, rhs) ((lhs) == (rhs))
#define EMITC_NE(lhs, rhs) ((lhs) != (rhs))
#define EMITC_OR(lhs, rhs) ((lhs) || (rhs))

#define EMITC_ADD(lhs, rhs) ((lhs) + (rhs))

#endif  // IREE_VM_OPS_EMITC_H_
