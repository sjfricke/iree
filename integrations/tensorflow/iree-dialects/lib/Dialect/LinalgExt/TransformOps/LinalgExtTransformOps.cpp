// Copyright 2022 The IREE Authors
//
// Licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "iree-dialects/Dialect/LinalgExt/TransformOps/LinalgExtTransformOps.h"
#include "iree-dialects/Dialect/LinalgExt/IR/LinalgExtOps.h"
#include "iree-dialects/Dialect/LinalgExt/Transforms/Transforms.h"
#include "iree-dialects/Dialect/LinalgTransform/SimplePatternRewriter.h"
#include "mlir/Dialect/Utils/StaticValueUtils.h"
#include "mlir/IR/OpImplementation.h"
#include "llvm/Support/FormatVariadic.h"

using namespace mlir;
using namespace mlir::iree_compiler::IREE;

LinalgExt::LinalgExtTransformOpsExtension::LinalgExtTransformOpsExtension() {
  registerTransformOps<
#define GET_OP_LIST
#include "iree-dialects/Dialect/LinalgExt/TransformOps/LinalgExtTransformOps.cpp.inc"
      >();
}

//===---------------------------------------------------------------------===//
// Utility functions
//===---------------------------------------------------------------------===//

/// Extracts a vector of int64_t from an array attribute. Asserts if the
/// attribute contains values other than integers.
static SmallVector<int64_t> extractI64Array(ArrayAttr attr) {
  SmallVector<int64_t> result;
  result.reserve(attr.size());
  for (APInt value : attr.getAsValueRange<IntegerAttr>())
    result.push_back(value.getSExtValue());
  return result;
}

//===---------------------------------------------------------------------===//
// FuseProducersOp
//===---------------------------------------------------------------------===//

DiagnosedSilenceableFailure
LinalgExt::FuseProducersOp::apply(transform::TransformResults &transformResults,
                                  transform::TransformState &state) {
  SmallVector<int64_t> operandsToFuse = extractI64Array(getOperandsToFuse());
  LinalgExt::LinalgExtFusionPattern pattern(getContext(), operandsToFuse);
  size_t numProducers = operandsToFuse.size();

  SmallVector<Operation *> transformedOps;
  SmallVector<SmallVector<Operation *>> fusedOps(numProducers);
  for (Operation *target : state.getPayloadOps(getTarget())) {
    // Apply the pattern.
    SimplePatternRewriter rewriter(target);
    FailureOr<LinalgExt::FusionResult> result =
        pattern.returningMatchAndRewrite(target, rewriter);
    if (failed(result))
      return DiagnosedSilenceableFailure::definiteFailure();

    // Update the fused operations.
    transformedOps.push_back(result->consumerOp);
    for (size_t i = 0; i < numProducers; ++i)
      fusedOps[i].push_back(result->fusedOps[i]);
  }

  transformResults.set(getTransformed().cast<OpResult>(), transformedOps);
  for (size_t i = 0; i < numProducers; ++i)
    transformResults.set(getFusedOps()[i], fusedOps[i]);
  return DiagnosedSilenceableFailure::success();
}

LogicalResult LinalgExt::FuseProducersOp::verify() {
  SmallVector<int64_t> operandsToFuse = extractI64Array(getOperandsToFuse());
  llvm::SmallDenseSet<int64_t> operandsSet;
  for (int64_t operandToFuse : operandsToFuse) {
    if (operandToFuse < 0) {
      return emitOpError() << "expects positive operand numbers, found "
                           << operandToFuse;
    }
    if (operandsSet.count(operandToFuse) != 0) {
      return emitOpError() << "expects unique operand numbers, found "
                           << operandToFuse << " multiple times";
    }
    operandsSet.insert(operandToFuse);
  }
  return success();
}

ParseResult LinalgExt::FuseProducersOp::parse(OpAsmParser &parser,
                                              OperationState &result) {
  OpAsmParser::UnresolvedOperand targetOperand;
  SMLoc opLoc;
  if (parser.getCurrentLocation(&opLoc))
    return failure();
  if (parser.parseOperand(targetOperand))
    return parser.emitError(opLoc, "expected `target` operand");
  if (parser.parseOptionalAttrDict(result.attributes))
    return failure();
  StringRef operandsToFuseAttrName("operands_to_fuse");
  Attribute operandsToFuseAttr = result.attributes.get(operandsToFuseAttrName);
  if (!operandsToFuseAttr) {
    return parser.emitError(opLoc, llvm::formatv("expected `{0}` attribute",
                                                 operandsToFuseAttrName));
  }
  auto operandsToFuseArrayAttr = operandsToFuseAttr.dyn_cast<ArrayAttr>();
  if (!operandsToFuseArrayAttr) {
    return parser.emitError(opLoc,
                            llvm::formatv("`{0}` attribute must be an array",
                                          operandsToFuseAttrName));
  }
  Type pdlOpType = parser.getBuilder().getType<pdl::OperationType>();
  size_t numProducers = operandsToFuseArrayAttr.size();
  result.addTypes(SmallVector<Type>(numProducers + 1, pdlOpType));
  if (parser.resolveOperand(targetOperand, pdlOpType, result.operands))
    return failure();
  return success();
}

void LinalgExt::FuseProducersOp::print(OpAsmPrinter &p) {
  p << ' ';
  p << getTarget();
  p.printOptionalAttrDict((*this)->getAttrs());
}

FailureOr<SmallVector<Operation *>>
LinalgExt::TileToForeachOp::applyToOne(::mlir::linalg::LinalgOp target,
                                       transform::TransformState &state) {
  linalg::LinalgTilingOptions tilingOptions;
  SmallVector<int64_t> numThreads = extractI64Array(getNumThreads());
  if (!numThreads.empty())
    tilingOptions.setTileSizes(numThreads);

  LinalgExt::ForeachThreadTilingPattern pattern(
      this->getContext(), tilingOptions,
      extractFromI64ArrayAttr(getThreadDimMapping()));
  // Apply the pattern.
  SimplePatternRewriter rewriter(target);
  FailureOr<iree_compiler::IREE::LinalgExt::TilingResult> result =
      pattern.returningMatchAndRewrite(
          cast<TilingInterface>(target.getOperation()), rewriter);
  if (failed(result)) {
    target->emitError("Failed to tile op to scf.foreach_thread:\n") << target;
    return failure();
  }
  return SmallVector<Operation *>{result->tileOp, result->tiledOp};
}

DiagnosedSilenceableFailure
LinalgExt::FuseIntoContainingOp::apply(transform::TransformResults &results,
                                       transform::TransformState &state) {
  ArrayRef<Operation *> producerOps = state.getPayloadOps(getProducerOp());
  ArrayRef<Operation *> containingOps = state.getPayloadOps(getContainingOp());
  for (auto it : llvm::zip(producerOps, containingOps)) {
    auto producerOp = dyn_cast<linalg::LinalgOp>(std::get<0>(it));
    Operation *containingOp = std::get<1>(it);
    if (!producerOp) {
      std::get<0>(it)->emitError("Cannot fuse op: Not a LinalgOp");
      return DiagnosedSilenceableFailure::definiteFailure();
    }
    LinalgExt::LinalgExtFusionInContainingOpPattern pattern(this->getContext(),
                                                            containingOp);

    // Apply the pattern.
    SimplePatternRewriter rewriter(producerOp);
    if (failed(pattern.returningMatchAndRewrite(producerOp, rewriter)))
      return DiagnosedSilenceableFailure::definiteFailure();
  }
  return DiagnosedSilenceableFailure::success();
}

FailureOr<Operation *> LinalgExt::RewriteForeachThreadToAsyncOp::applyToOne(
    scf::ForeachThreadOp target, transform::TransformState &state) {
  LinalgExt::ForeachThreadOpToAsyncRewriter pattern(this->getContext());
  SimplePatternRewriter rewriter(target);
  return pattern.returningMatchAndRewrite(target, rewriter);
}

FailureOr<scf::ForOp> LinalgExt::RewriteForeachThreadToScfForOp::applyToOne(
    scf::ForeachThreadOp target, transform::TransformState &state) {
  LinalgExt::ForeachThreadOpToScfForRewriter pattern(this->getContext());
  SimplePatternRewriter rewriter(target);
  return pattern.returningMatchAndRewrite(target, rewriter);
}

#define GET_OP_CLASSES
#include "iree-dialects/Dialect/LinalgExt/TransformOps/LinalgExtTransformOps.cpp.inc"
