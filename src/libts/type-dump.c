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

#include "flang/common.h"
#include "flang/libts.h"
#include "flang/debug.h"
#include "flang/libast.h"

// TODO add a new param, that will require 'debug' method call
char* ty_to_color(u64 ty_id) {
  if (ty_id == TS_STRING) {
    return "\x1B[32m";
  }
  ty_t ty = ts_type_table[ty_id];
  switch (ty.of) {
  case FL_NUMBER:
  case FL_POINTER:
    return "\x1B[33m";
  case FL_VECTOR:
    return "\x1B[30m";
  case FL_STRUCT:
    return "\x1B[31m";
  case FL_FUNCTION:
    return "\x1B[32m";
  default:
    return "\x1B[32m";
  }
}
char* ty_to_printf(u64 ty_id) {
  // only builtin atm
  switch (ty_id) {
  case TS_I8:
  case TS_I16:
    return "%d";
  case TS_U8:
  case TS_U16:
    return "%u";
  case TS_I32:
  case TS_I64:
    return "%ld";
  case TS_U32:
  case TS_U64:
    return "%zu";
  case TS_F32:
  case TS_F64:
    return "%f";
  case TS_STRING:
    // case TS_CSTR:
    return "%s";
  }

  ty_t ty = ts_type_table[ty_id];
  switch (ty.of) {
  case FL_POINTER:
  case FL_VECTOR:
    return "%p";
  case FL_STRUCT:
    return "%s";
  case FL_FUNCTION:
    return "%s";
  default:
    return "";
  }
}

string* ty_to_string(u64 ty_id) {
  ty_t ty = ts_type_table[ty_id];
  // cached?
  if (ty.decl) {
    return ty.decl;
  }

  string* buffer = st_new(64, st_enc_utf8);

  switch (ty.of) {
  case FL_POINTER:
    st_append_c(&buffer, "ptr(");
    st_append(&buffer, ty_to_string(ty.ptr.to));
    st_append_c(&buffer, ")");
    break;
  case FL_VECTOR:
    st_append_c(&buffer, "vector(");
    st_append(&buffer, ty_to_string(ty.vector.to));
    st_append_c(&buffer, ")");
    break;
  case FL_STRUCT: {
    st_append_c(&buffer, "struct ");
    st_append(&buffer, ty.id);
    st_append_c(&buffer, " { ");

    u64 i;
    for (i = 0; i < ty.structure.nfields; ++i) {
      st_append(&buffer, ty_to_string(ty.structure.fields[i]));
      st_append_c(&buffer, " ");
      st_append(&buffer, ty.structure.properties[i]);
      st_append_c(&buffer, ", ");
    }
    st_append_c(&buffer, "}");
  } break;
  case FL_FUNCTION: {
    st_append_c(&buffer, "fn ");
    st_append_c(&buffer, ty.id ? ty.id->value : "Anonymous");
    st_append_c(&buffer, " (");

    u64 i;
    for (i = 0; i < ty.func.nparams; ++i) {
      st_append(&buffer, ty_to_string(ty.func.params[i]));
      st_append_c(&buffer, ", ");
    }

    st_append_c(&buffer, ") : ");
    st_append(&buffer, ty_to_string(ty.func.ret));
  } break;
  default: {} // remove warning
  }
  return ty.decl = buffer;
}

// TODO buffered version
void ty_dump(u64 ty_id) {
  ty_t ty = ts_type_table[ty_id];

  log_debug2("[%zu]", ty_id);

  switch (ty.of) {
  case FL_VOID:
    log_debug2("void");
    break;
  case FL_NUMBER:
    log_debug2("%s", ty.id->value);
    break;
  case FL_POINTER:
    log_debug2("ptr<");
    ty_dump(ty.ptr.to);
    log_debug2(">");
    break;
  case FL_VECTOR:
    log_debug2("vector<");
    ty_dump(ty.vector.to);
    log_debug2(">");
    break;
  case FL_STRUCT: {
    log_debug2("struct %s {",
               ty.structure.decl->structure.id->identifier.string->value);
    u64 i;
    for (i = 0; i < ty.structure.nfields; ++i) {
      ty_dump(ty.structure.fields[i]);
      log_debug2(", ");
    }
    log_debug2("}");
  } break;
  case FL_FUNCTION: {
    log_debug2("fn %s(", ty.id ? ty.id->value : "Anonymous");
    u64 i;
    for (i = 0; i < ty.func.nparams; ++i) {
      ty_dump(ty.func.params[i]);
      log_debug2(", ");
    }

    log_debug2(") : ");
    ty_dump(ty.func.ret);
  } break;
  case FL_INFER: {
    log_debug("Unknown");
  } break;
  default: { log_error("ty_dump(%u) not implement", ty.of); }
  }
}

void __ty_dump_cell(u64 ty_id, int indent) {
  ty_t ty = ts_type_table[ty_id];

  switch (ty.of) {
  case FL_VOID:
    log_debug("%*s[%zu] Void", indent, " ", ty_id);
    break;
  case FL_NUMBER:
    log_debug("%*s[%zu] Number (fp %d, bits %d, sign %d)", indent, " ", ty_id,
              ty.number.fp, ty.number.bits, ty.number.sign);
    break;
  case FL_POINTER:
    log_debug("%*s[%zu] Pointer -> %zu", indent, " ", ty_id, ty.ptr.to);
    __ty_dump_cell(ty.ptr.to, indent + 2);
    break;
  case FL_VECTOR:
    log_debug("%*s[%zu] Vector -> %zu", indent, " ", ty_id, ty.ptr.to);
    __ty_dump_cell(ty.vector.to, indent + 2);
    break;
  case FL_STRUCT: {
    log_debug("%*s[%zu] Struct [%s]", indent, " ", ty_id,
              ty.structure.decl->structure.id->identifier.string->value);
    u64 i;
    for (i = 0; i < ty.structure.nfields; ++i) {
      __ty_dump_cell(ty.structure.fields[i], indent + 2);
    }
  } break;
  case FL_FUNCTION: {
    log_debug("%*sFunction [%s] arity(%zu) -> [%zu]", indent, " ",
              ty.id ? ty.id->value : "Anonymous", ty.func.nparams, ty.func.ret);
    u64 i;
    __ty_dump_cell(ty.func.ret, indent + 2);
    for (i = 0; i < ty.func.nparams; ++i) {
      __ty_dump_cell(ty.func.params[i], indent + 2);
    }
  } break;
  case FL_INFER: {
    log_debug("%*sUnknown", indent, " ");
  } break;
  default: { log_error("ty_dump(%u) not implement", ty.of); }
  }
}

void ty_dump_table() {
  u64 i;

  for (i = 0; i < ts_type_size_s; ++i) {
    __ty_dump_cell(i, 0);
  }
}