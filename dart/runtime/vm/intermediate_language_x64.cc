// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "vm/globals.h"  // Needed here to get TARGET_ARCH_X64.
#if defined(TARGET_ARCH_X64)

#include "lib/error.h"
#include "vm/flow_graph_compiler.h"
#include "vm/locations.h"
#include "vm/object_store.h"
#include "vm/stub_code.h"

#define __ compiler->assembler()->

namespace dart {


// True iff. the arguments to a call will be properly pushed and can
// be popped after the call.
template <typename T> static bool VerifyCallComputation(T* comp) {
  // Argument values should be consecutive temps.
  //
  // TODO(kmillikin): implement stack height tracking so we can also assert
  // they are on top of the stack.
  intptr_t previous = -1;
  for (int i = 0; i < comp->ArgumentCount(); ++i) {
    Value* val = comp->ArgumentAt(i);
    if (!val->IsUse()) return false;
    intptr_t current = val->AsUse()->definition()->temp_index();
    if (i != 0) {
      if (current != (previous + 1)) return false;
    }
    previous = current;
  }
  return true;
}


void BindInstr::EmitNativeCode(FlowGraphCompiler* compiler) {
  computation()->EmitNativeCode(compiler);
  __ pushq(locs()->out().reg());
}


static LocationSummary* MakeSimpleLocationSummary(
    intptr_t input_count, Location out) {
  LocationSummary* summary = new LocationSummary(input_count, 0);
  for (intptr_t i = 0; i < input_count; i++) {
    summary->set_in(i, Location::RequiresRegister());
  }
  summary->set_out(out);
  return summary;
}


LocationSummary* CurrentContextComp::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(0, Location::RequiresRegister());
}


void CurrentContextComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  __ movq(locs()->out().reg(), CTX);
}


LocationSummary* StoreContextComp::MakeLocationSummary() const {
  LocationSummary* summary = new LocationSummary(1, 0);
  summary->set_in(0, Location::RegisterLocation(CTX));
  return summary;
}


void StoreContextComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  // Nothing to do.  Context register were loaded by register allocator.
  ASSERT(locs()->in(0).reg() == CTX);
}


LocationSummary* StrictCompareComp::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(2, Location::SameAsFirstInput());
}


void StrictCompareComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  const Bool& bool_true = Bool::ZoneHandle(Bool::True());
  const Bool& bool_false = Bool::ZoneHandle(Bool::False());

  Register left = locs()->in(0).reg();
  Register right = locs()->in(1).reg();
  Register result = locs()->out().reg();

  __ cmpq(left, right);
  Label load_true, done;
  if (kind() == Token::kEQ_STRICT) {
    __ j(EQUAL, &load_true, Assembler::kNearJump);
  } else {
    __ j(NOT_EQUAL, &load_true, Assembler::kNearJump);
  }
  __ LoadObject(result, bool_false);
  __ jmp(&done, Assembler::kNearJump);
  __ Bind(&load_true);
  __ LoadObject(result, bool_true);
  __ Bind(&done);
}


// Generic summary for call instructions that have all arguments pushed
// on the stack and return the result in a fixed register RAX.
static LocationSummary* MakeCallSummary() {
  LocationSummary* result = new LocationSummary(0, 0);
  result->set_out(Location::RegisterLocation(RAX));
  return result;
}


LocationSummary* ClosureCallComp::MakeLocationSummary() const {
  return MakeCallSummary();
}


void ClosureCallComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  ASSERT(VerifyCallComputation(this));
  // The arguments to the stub include the closure.  The arguments
  // descriptor describes the closure's arguments (and so does not include
  // the closure).
  int argument_count = ArgumentCount();
  const Array& arguments_descriptor =
      CodeGenerator::ArgumentsDescriptor(argument_count - 1,
                                         argument_names());
  __ LoadObject(R10, arguments_descriptor);

  compiler->GenerateCall(token_index(),
                         try_index(),
                         &StubCode::CallClosureFunctionLabel(),
                         PcDescriptors::kOther);
  __ Drop(argument_count);
}


LocationSummary* InstanceCallComp::MakeLocationSummary() const {
  return MakeCallSummary();
}


void InstanceCallComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  ASSERT(VerifyCallComputation(this));
  compiler->AddCurrentDescriptor(PcDescriptors::kDeopt,
                                 cid(),
                                 token_index(),
                                 try_index());
  compiler->EmitInstanceCall(cid(),
                             token_index(),
                             try_index(),
                             function_name(),
                             ArgumentCount(),
                             argument_names(),
                             checked_argument_count());
}


LocationSummary* StaticCallComp::MakeLocationSummary() const {
  return MakeCallSummary();
}


void StaticCallComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  ASSERT(VerifyCallComputation(this));
  compiler->EmitStaticCall(token_index(),
                           try_index(),
                           function(),
                           ArgumentCount(),
                           argument_names());
}


LocationSummary* LoadLocalComp::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(0, Location::RequiresRegister());
}


void LoadLocalComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  Register result = locs()->out().reg();
  __ movq(result, Address(RBP, local().index() * kWordSize));
}


LocationSummary* StoreLocalComp::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(1, Location::SameAsFirstInput());
}


void StoreLocalComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  Register value = locs()->in(0).reg();
  Register result = locs()->out().reg();
  ASSERT(result == value);  // Assert that register assignment is correct.
  __ movq(Address(RBP, local().index() * kWordSize), value);
}


LocationSummary* ConstantVal::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(0, Location::RequiresRegister());
}


void ConstantVal::EmitNativeCode(FlowGraphCompiler* compiler) {
  Register result = locs()->out().reg();
  if (value().IsSmi()) {
    int64_t imm = reinterpret_cast<int64_t>(value().raw());
    __ movq(result, Immediate(imm));
  } else {
    __ LoadObject(result, value());
  }
}


LocationSummary* UseVal::MakeLocationSummary() const {
  return NULL;
}


void UseVal::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* AssertAssignableComp::MakeLocationSummary() const {
  LocationSummary* summary = new LocationSummary(3, 0);
  summary->set_in(0, Location::RegisterLocation(RAX));
  summary->set_in(1, Location::RegisterLocation(RCX));
  summary->set_in(2, Location::RegisterLocation(RDX));
  summary->set_out(Location::RegisterLocation(RAX));
  return summary;
}


void AssertAssignableComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  ASSERT(locs()->in(0).reg() == RAX);  // Value.
  ASSERT(locs()->in(1).reg() == RCX);  // Instantiator.
  ASSERT(locs()->in(2).reg() == RDX);  // Instantiator type arguments.

  compiler->GenerateAssertAssignable(cid(),
                                     token_index(),
                                     try_index(),
                                     dst_type(),
                                     dst_name());
  ASSERT(locs()->in(0).reg() == locs()->out().reg());
}


LocationSummary* AssertBooleanComp::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(1, Location::SameAsFirstInput());
}


void AssertBooleanComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  Register obj = locs()->in(0).reg();
  Register result = locs()->out().reg();

  // Check that the type of the value is allowed in conditional context.
  // Call the runtime if the object is not bool::true or bool::false.
  Label done;
  __ CompareObject(obj, Bool::ZoneHandle(Bool::True()));
  __ j(EQUAL, &done, Assembler::kNearJump);
  __ CompareObject(obj, Bool::ZoneHandle(Bool::False()));
  __ j(EQUAL, &done, Assembler::kNearJump);

  __ pushq(Immediate(Smi::RawValue(token_index())));  // Source location.
  __ pushq(obj);  // Push the source object.
  compiler->GenerateCallRuntime(cid(),
                                token_index(),
                                try_index(),
                                kConditionTypeErrorRuntimeEntry);
  // We should never return here.
  __ int3();

  __ Bind(&done);
  ASSERT(obj == result);
}


LocationSummary* EqualityCompareComp::MakeLocationSummary() const {
  return NULL;
}


void EqualityCompareComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* NativeCallComp::MakeLocationSummary() const {
  return NULL;
}


void NativeCallComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* StoreIndexedComp::MakeLocationSummary() const {
  return NULL;
}


void StoreIndexedComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* InstanceSetterComp::MakeLocationSummary() const {
  return NULL;
}


void InstanceSetterComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* StaticSetterComp::MakeLocationSummary() const {
  return NULL;
}


void StaticSetterComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* LoadInstanceFieldComp::MakeLocationSummary() const {
  return NULL;
}


void LoadInstanceFieldComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* StoreInstanceFieldComp::MakeLocationSummary() const {
  return NULL;
}


void StoreInstanceFieldComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* LoadStaticFieldComp::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(0, Location::RequiresRegister());
}


void LoadStaticFieldComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  Register result = locs()->out().reg();
  __ LoadObject(result, field());
  __ movq(result, FieldAddress(result, Field::value_offset()));
}


LocationSummary* StoreStaticFieldComp::MakeLocationSummary() const {
  LocationSummary* locs = new LocationSummary(1, 1);
  locs->set_in(0, Location::RequiresRegister());
  locs->set_temp(0, Location::RequiresRegister());
  locs->set_out(Location::SameAsFirstInput());
  return locs;
}


void StoreStaticFieldComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  Register value = locs()->in(0).reg();
  Register temp = locs()->temp(0).reg();
  ASSERT(locs()->out().reg() == value);

  __ LoadObject(temp, field());
  __ StoreIntoObject(temp, FieldAddress(temp, Field::value_offset()), value);
}


LocationSummary* BooleanNegateComp::MakeLocationSummary() const {
  return NULL;
}


void BooleanNegateComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* InstanceOfComp::MakeLocationSummary() const {
  return NULL;
}


void InstanceOfComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* CreateArrayComp::MakeLocationSummary() const {
  return NULL;
}


void CreateArrayComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* CreateClosureComp::MakeLocationSummary() const {
  return NULL;
}


void CreateClosureComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* AllocateObjectComp::MakeLocationSummary() const {
  return MakeCallSummary();
}


void AllocateObjectComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  const Class& cls = Class::ZoneHandle(constructor().owner());
  const Code& stub = Code::Handle(StubCode::GetAllocationStubForClass(cls));
  const ExternalLabel label(cls.ToCString(), stub.EntryPoint());
  compiler->GenerateCall(token_index(),
                         try_index(),
                         &label,
                         PcDescriptors::kOther);
  __ Drop(arguments().length());  // Discard arguments.
}


LocationSummary* AllocateObjectWithBoundsCheckComp::
    MakeLocationSummary() const {
  return MakeSimpleLocationSummary(2, Location::RequiresRegister());
}


void AllocateObjectWithBoundsCheckComp::EmitNativeCode(
    FlowGraphCompiler* compiler) {
  const Class& cls = Class::ZoneHandle(constructor().owner());
  Register type_arguments = locs()->in(0).reg();
  Register instantiator_type_arguments = locs()->in(1).reg();
  Register result = locs()->out().reg();

  // Push the result place holder initialized to NULL.
  __ PushObject(Object::ZoneHandle());
  __ pushq(Immediate(Smi::RawValue(token_index())));
  __ PushObject(cls);
  __ pushq(type_arguments);
  __ pushq(instantiator_type_arguments);
  compiler->GenerateCallRuntime(cid(),
                                token_index(),
                                try_index(),
                                kAllocateObjectWithBoundsCheckRuntimeEntry);
  // Pop instantiator type arguments, type arguments, class, and
  // source location.
  __ Drop(4);
  __ popq(result);  // Pop new instance.
}


LocationSummary* LoadVMFieldComp::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(1, Location::RequiresRegister());
}


void LoadVMFieldComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  Register obj = locs()->in(0).reg();
  Register result = locs()->out().reg();

  __ movq(result, FieldAddress(obj, offset_in_bytes()));
}


LocationSummary* StoreVMFieldComp::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(2, Location::SameAsFirstInput());
}


void StoreVMFieldComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  Register value_reg = locs()->in(0).reg();
  Register dest_reg = locs()->in(1).reg();
  ASSERT(value_reg == locs()->out().reg());

  __ StoreIntoObject(dest_reg, FieldAddress(dest_reg, offset_in_bytes()),
                     value_reg);
}


LocationSummary* InstantiateTypeArgumentsComp::MakeLocationSummary() const {
  return NULL;
}


void InstantiateTypeArgumentsComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* ExtractConstructorTypeArgumentsComp::
    MakeLocationSummary() const {
  return NULL;
}


void ExtractConstructorTypeArgumentsComp::EmitNativeCode(
    FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* ExtractConstructorInstantiatorComp::
    MakeLocationSummary() const {
  return NULL;
}


void ExtractConstructorInstantiatorComp::EmitNativeCode(
    FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* AllocateContextComp::MakeLocationSummary() const {
  return NULL;
}


void AllocateContextComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* ChainContextComp::MakeLocationSummary() const {
  return NULL;
}


void ChainContextComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* CloneContextComp::MakeLocationSummary() const {
  return NULL;
}


void CloneContextComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* CatchEntryComp::MakeLocationSummary() const {
  return NULL;
}


void CatchEntryComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  UNIMPLEMENTED();
}


LocationSummary* BinaryOpComp::MakeLocationSummary() const {
  return MakeSimpleLocationSummary(2, Location::SameAsFirstInput());
}


// TODO(srdjan): Implement variations.
static bool TryEmitSmiBinaryOp(FlowGraphCompiler* compiler,
                               BinaryOpComp* comp) {
  ASSERT((comp->ic_data() != NULL));
  const ICData& ic_data = *comp->ic_data();
  if (ic_data.IsNull()) return false;
  if (ic_data.num_args_tested() != 2) return false;
  if (ic_data.NumberOfChecks() != 1) return false;
  Function& target = Function::Handle();
  GrowableArray<const Class*> classes;
  ic_data.GetCheckAt(0, &classes, &target);
  const Class& smi_class =
      Class::Handle(Isolate::Current()->object_store()->smi_class());
  if ((classes[0]->raw() != smi_class.raw()) ||
      (classes[1]->raw() != smi_class.raw())) {
    return false;
  }
  // TODO(srdjan): need to allocate a temporary register (now using r10)
  Register left = comp->locs()->in(0).reg();
  Register right = comp->locs()->in(1).reg();
  Register result = comp->locs()->out().reg();
  Register temp = R10;
  Label* deopt = compiler->AddDeoptStub(comp->instance_call()->cid(),
                                        comp->instance_call()->token_index(),
                                        comp->instance_call()->try_index(),
                                        kDeoptSmiBinaryOp,
                                        temp,
                                        right);
  __ movq(temp, left);
  __ orq(left, right);
  __ testq(left, Immediate(kSmiTagMask));
  __ j(NOT_ZERO, deopt);
  __ movq(left, temp);
  switch (comp->op_kind()) {
    case Token::kADD: {
      __ addq(left, right);
      __ j(OVERFLOW, deopt);
      if (result != left) {
        __ movq(result, left);
      }
      break;
    }
    default:
      UNREACHABLE();
  }
  return true;
}

void BinaryOpComp::EmitNativeCode(FlowGraphCompiler* compiler) {
  if (TryEmitSmiBinaryOp(compiler, this)) {
    // Operation inlined.
    return;
  }
  // TODO(srdjan): Remove this code once BinaryOpComp has been implemeneted
  // for all intended operations.
  Register left = locs()->in(0).reg();
  Register right = locs()->in(1).reg();
  __ pushq(left);
  __ pushq(right);
  InstanceCallComp* instance_call_comp = instance_call();
  instance_call_comp->EmitNativeCode(compiler);
  if (locs()->out().reg() != RAX) {
    __ movq(locs()->out().reg(), RAX);
  }
}

}  // namespace dart

#undef __

#endif  // defined TARGET_ARCH_X64
