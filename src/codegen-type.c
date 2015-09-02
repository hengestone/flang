/*
* Copyright 2015 Luis Lafuente <llafuente@noboxout.com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "flang.h"

LLVMTypeRef fl_codegen_get_type(fl_ast_t* node, LLVMContextRef context) {
  return fl_codegen_get_typeid(node->ty_id, context);
}

LLVMTypeRef fl_codegen_get_typeid(size_t id, LLVMContextRef context) {
  // TODO codegen is cached?
  fl_type_t* t = &fl_type_table[id];

  if (t->codegen) {
    return (LLVMTypeRef)t->codegen;
  }

  log_verbose("llvm for typeid = %zu", id);

  switch (t->of) {
  case FL_VOID:
    t->codegen = (void*)LLVMVoidType();
    break;
  case FL_NUMBER:
    if (t->number.fp) {
      switch (t->number.bits) {
      case 32:
        t->codegen = (void*)LLVMFloatType();
        break;
      case 64:
        t->codegen = (void*)LLVMDoubleType();
        break;
      }
    } else {
      log_silly("t.number.bits %d", t->number.bits);

      t->codegen = (void*)LLVMIntType(t->number.bits);
    }
    break;
  case FL_POINTER:
    t->codegen =
        (void*)LLVMPointerType(fl_codegen_get_typeid(t->ptr.to, context), 0);
    break;
  case FL_STRUCT: {
    log_verbose("codegen struct '%s'", t->id->value);
    t->codegen = LLVMStructCreateNamed(context, t->id->value);
    // create the list!
    fl_ast_t* l = t->structure.decl->structure.fields;

    size_t i;
    size_t count = t->structure.nfields;
    LLVMTypeRef* types = malloc(count * sizeof(LLVMTypeRef));
    for (i = 0; i < count; ++i) {
      // types[i] = fl_codegen_get_typeid(l->list.elements[i]->ty_id, context);
      types[i] = fl_codegen_get_typeid(t->structure.fields[i], context);
    }
    LLVMStructSetBody(t->codegen, types, count, 0);
    free(types);
  } break;
  case FL_VECTOR: {
    t->codegen = LLVMArrayType(fl_codegen_get_typeid(t->vector.to, context),
                               t->vector.length);
  } break;
  default: {
    fl_print_type(id, 0);
    log_error("type not handled yet [%zu]", id);
  }
  }

  if (!t->codegen) {
    log_error("cannot find LLVM-type");
  }

  return (LLVMTypeRef)t->codegen;
}

LLVMValueRef fl_codegen_cast_op(LLVMBuilderRef builder, size_t current,
                                size_t expected, LLVMValueRef value,
                                LLVMContextRef context) {
  log_debug("cast [%zu == %zu] [%p]", expected, current, value);

  if (expected == current) {
    return value;
  }

  fl_type_t cu_type = fl_type_table[current];
  fl_type_t ex_type = fl_type_table[expected];

  if (ex_type.of == cu_type.of) {
    switch (ex_type.of) {
    case FL_NUMBER:
      // fpto*i
      if (cu_type.number.fp && !ex_type.number.fp) {
        log_verbose("fptosi");

        if (ex_type.number.sign) {
          return LLVMBuildFPToSI(
              builder, value, fl_codegen_get_typeid(expected, context), "cast");
        }

        return LLVMBuildFPToUI(
            builder, value, fl_codegen_get_typeid(expected, context), "cast");
      }
      // *itofp
      if (!cu_type.number.fp && ex_type.number.fp) {
        log_verbose("sitofp");

        if (ex_type.number.sign) {
          return LLVMBuildSIToFP(
              builder, value, fl_codegen_get_typeid(expected, context), "cast");
        }

        return LLVMBuildUIToFP(
            builder, value, fl_codegen_get_typeid(expected, context), "cast");
      }

      // LLVMBuildFPTrunc

      bool fp = cu_type.number.fp;

      // upcast
      if (cu_type.number.bits < ex_type.number.bits) {
        log_verbose("upcast");

        if (fp) {
          return LLVMBuildFPExt(
              builder, value, fl_codegen_get_typeid(expected, context), "cast");
        }

        // sign -> sign
        if ((cu_type.number.sign && ex_type.number.sign) ||
            (!cu_type.number.sign && ex_type.number.sign)) {
          return LLVMBuildSExt(
              builder, value, fl_codegen_get_typeid(expected, context), "cast");
        }

        return LLVMBuildZExt(builder, value,
                             fl_codegen_get_typeid(expected, context), "cast");
      }

      // downcast / truncate
      if (cu_type.number.bits > ex_type.number.bits) {
        log_verbose("downcast");

        if (fp) {
          return LLVMBuildFPTrunc(
              builder, value, fl_codegen_get_typeid(expected, context), "cast");
        }
        return LLVMBuildTrunc(builder, value,
                              fl_codegen_get_typeid(expected, context), "cast");
      }
      break;
    default: {
      // TODO more friendly
      log_error("invalid cast of type %zu to %zu", current, expected);
    }
    }
  }

  log_error("invalid casting");
  return 0;
}
