// Copyright 2019 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef IREE_MODULES_HAL_MODULE_H_
#define IREE_MODULES_HAL_MODULE_H_

#include <stdint.h>

#include "iree/base/api.h"
#include "iree/hal/api.h"
#include "iree/vm/api.h"

IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_allocator, iree_hal_allocator_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_buffer, iree_hal_buffer_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_buffer_view, iree_hal_buffer_view_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_command_buffer,
                              iree_hal_command_buffer_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_descriptor_set,
                              iree_hal_descriptor_set_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_descriptor_set_layout,
                              iree_hal_descriptor_set_layout_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_device, iree_hal_device_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_event, iree_hal_event_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_executable, iree_hal_executable_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_executable_cache,
                              iree_hal_executable_cache_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_executable_layout,
                              iree_hal_executable_layout_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_fence, iree_hal_fence_t);
IREE_VM_DECLARE_TYPE_ADAPTERS(iree_hal_semaphore, iree_hal_semaphore_t);

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

enum iree_hal_module_flag_bits_t {
  IREE_HAL_MODULE_FLAG_NONE = 0u,

  // Forces HAL methods to block instead of yielding as a coroutine.
  IREE_HAL_MODULE_FLAG_SYNCHRONOUS = 1u << 0,
};
typedef uint32_t iree_hal_module_flags_t;

// Registers the custom types used by the HAL module.
// WARNING: not thread-safe; call at startup before using.
IREE_API_EXPORT iree_status_t iree_hal_module_register_types(void);

// Creates the HAL module initialized to use a specific |device|.
// Each context using this module will share the device and have compatible
// allocations.
IREE_API_EXPORT iree_status_t iree_hal_module_create(
    iree_hal_device_t* device, iree_hal_module_flags_t flags,
    iree_allocator_t host_allocator, iree_vm_module_t** out_module);

// Returns the device currently in use by the HAL module.
// Returns NULL if no device has been initialized yet.
IREE_API_EXPORT iree_hal_device_t* iree_hal_module_state_device(
    iree_vm_module_state_t* module_state);

// TODO(benvanik): generate these list helpers:

IREE_API_EXPORT iree_hal_buffer_view_t* iree_vm_list_get_buffer_view_assign(
    const iree_vm_list_t* list, iree_host_size_t i);

IREE_API_EXPORT iree_hal_buffer_view_t* iree_vm_list_get_buffer_view_retain(
    const iree_vm_list_t* list, iree_host_size_t i);

IREE_API_EXPORT iree_status_t iree_vm_list_set_buffer_view_retain(
    iree_vm_list_t* list, iree_host_size_t i, iree_hal_buffer_view_t* value);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // IREE_MODULES_HAL_MODULE_H_
