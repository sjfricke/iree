// RUN: iree-opt %s -iree-transform-dialect-interpreter -transform-dialect-drop-schedule | FileCheck %s

#device_target_cpu = #hal.device.target<"cpu", {executable_targets = [#hal.executable.target<"llvm", "embedded-elf-x86_64", {cpu_features = "", data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", native_vector_size = 16 : index, target_triple = "x86_64-unknown-unknown-eabi-elf"}>]}>
#executable_layout = #hal.executable.layout<push_constants = 0, sets = [#hal.descriptor_set.layout<0, bindings = [#hal.descriptor_set.binding<0, storage_buffer>, #hal.descriptor_set.binding<1, storage_buffer>, #hal.descriptor_set.binding<2, storage_buffer>]>]>
#executable_target_embedded_elf_x86_64_ = #hal.executable.target<"llvm", "embedded-elf-x86_64", {cpu_features = "", data_layout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128", native_vector_size = 16 : index, target_triple = "x86_64-unknown-unknown-eabi-elf"}>

hal.executable private @pad_matmul_static_dispatch_0 {
  hal.executable.variant public @embedded_elf_x86_64, target = #executable_target_embedded_elf_x86_64_ {
    hal.executable.export public @pad_matmul_static_dispatch_0 ordinal(0) layout(#executable_layout)
    builtin.module {
      func.func @pad_matmul_static_dispatch_0() {
        %c0 = arith.constant 0 : index
        %0 = hal.interface.binding.subspan set(0) binding(0) type(storage_buffer) offset(%c0) alignment(64) : !flow.dispatch.tensor<readonly:250x500xf32>
        %1 = hal.interface.binding.subspan set(0) binding(1) type(storage_buffer) offset(%c0) alignment(64) : !flow.dispatch.tensor<readonly:500x1020xf32>
        %2 = hal.interface.binding.subspan set(0) binding(2) type(storage_buffer) offset(%c0) alignment(64) : !flow.dispatch.tensor<readwrite:250x1020xf32>
        %3 = flow.dispatch.tensor.load %0, offsets = [0, 0], sizes = [250, 500], strides = [1, 1] : !flow.dispatch.tensor<readonly:250x500xf32> -> tensor<250x500xf32>
        %4 = flow.dispatch.tensor.load %1, offsets = [0, 0], sizes = [500, 1020], strides = [1, 1] : !flow.dispatch.tensor<readonly:500x1020xf32> -> tensor<500x1020xf32>

        %50 = linalg.init_tensor [250, 1020] : tensor<250x1020xf32>
        %cst = arith.constant 0.000000e+00 : f32
        %5 = linalg.fill ins(%cst : f32) outs(%50 : tensor<250x1020xf32>) -> tensor<250x1020xf32>

        //      CHECK: memref.assume_alignment %{{.*}}, 64 : memref<250x1020xf32>
        // CHECK-NEXT: linalg.fill ins(%{{.*}} : f32) outs(%{{.*}} : memref<250x1020xf32>)
        // CHECK-NEXT: linalg.matmul{{.*}}ins(%{{.*}} : memref<250x500xf32>, memref<500x1020xf32>) outs(%{{.*}} : memref<250x1020xf32>)
        // CHECK-NEXT: return

        %6 = linalg.matmul ins(%3, %4 : tensor<250x500xf32>, tensor<500x1020xf32>) outs(%5 : tensor<250x1020xf32>) -> tensor<250x1020xf32>
        flow.dispatch.tensor.store %6, %2, offsets = [0, 0], sizes = [250, 1020], strides = [1, 1] : tensor<250x1020xf32> -> !flow.dispatch.tensor<readwrite:250x1020xf32>
        return
      }
    }
  }
}

transform.with_pdl_patterns {
^bb0(%arg0: !pdl.operation):
  pdl.pattern @pdl_matmul_target : benefit(1) {
    %args = operands
    %results = types
    %0 = operation "linalg.matmul"(%args : !pdl.range<value>) -> (%results : !pdl.range<type>)
    // TODO: we don't want this, but it is the required terminator for pdl.pattern
    rewrite %0 with "transform.dialect"
  }

  transform.structured.canonicalized_sequence %arg0 {
  ^bb1(%arg1: !pdl.operation):
    %0 = pdl_match @pdl_matmul_target in %arg1
    transform.iree.bufferize
  }
}
