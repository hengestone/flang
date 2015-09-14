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

void ast_traverse(ast_t* ast, fl_ast_cb_t cb, ast_t* parent, size_t level,
                  void* userdata) {
#define TRAVERSE(node)                                                         \
  if (node) {                                                                  \
    ast_traverse(node, cb, ast, level, userdata);                              \
  }

#define TRAVERSE_LIST(node)                                                    \
  {                                                                            \
    size_t i = 0;                                                              \
    ast_t* tmp;                                                                \
                                                                               \
    if (node) {                                                                \
      while ((tmp = node[i++]) != 0) {                                         \
        TRAVERSE(tmp);                                                         \
      }                                                                        \
    }                                                                          \
  }

  if (!ast) {
    log_warning("ast_traverse: (nil)");
    return;
  }

  ++level;
  // stop if callback is false
  if (!cb(ast, parent, level, userdata)) {
    return;
  }

  switch (ast->type) {
  case FL_AST_PROGRAM:
    TRAVERSE(ast->program.body);
    break;
  case FL_AST_BLOCK: {
    TRAVERSE_LIST(ast->block.body);
  } break;
  case FL_AST_LIST: {
    TRAVERSE_LIST(ast->list.elements);
  } break;
  case FL_AST_EXPR_ASSIGNAMENT:
    TRAVERSE(ast->assignament.left);
    TRAVERSE(ast->assignament.right);
    break;
  case FL_AST_EXPR_BINOP:
    TRAVERSE(ast->binop.left);
    TRAVERSE(ast->binop.right);
    break;
  case FL_AST_EXPR_LUNARY:
    TRAVERSE(ast->lunary.element);
    break;
  case FL_AST_EXPR_RUNARY:
    TRAVERSE(ast->runary.element);
    break;
  case FL_AST_EXPR_CALL: {
    TRAVERSE(ast->call.callee);
    TRAVERSE(ast->call.arguments);
  } break;
  case FL_AST_EXPR_MEMBER: {
    TRAVERSE(ast->member.left);
    TRAVERSE(ast->member.property);
  } break;
  case FL_AST_DTOR_VAR: {
    TRAVERSE(ast->var.id);
    TRAVERSE(ast->var.type);
  } break;
  case FL_AST_DECL_FUNCTION: {
    TRAVERSE(ast->func.id);
    TRAVERSE(ast->func.ret_type);
    TRAVERSE(ast->func.params);
    TRAVERSE(ast->func.body);
  } break;
  case FL_AST_PARAMETER: {
    TRAVERSE(ast->param.id);
    TRAVERSE(ast->param.type);
  } break;
  case FL_AST_STMT_RETURN: {
    TRAVERSE(ast->ret.argument);
  } break;
  case FL_AST_STMT_IF: {
    TRAVERSE(ast->if_stmt.test);
    TRAVERSE(ast->if_stmt.block);
    TRAVERSE(ast->if_stmt.alternate);
  } break;
  case FL_AST_STMT_LOOP: {
    TRAVERSE(ast->loop.init);
    TRAVERSE(ast->loop.pre_cond);
    TRAVERSE(ast->loop.update);
    TRAVERSE(ast->loop.block);
    TRAVERSE(ast->loop.post_cond);
  } break;
  case FL_AST_CAST: {
    TRAVERSE(ast->cast.element);
  } break;
  case FL_AST_DECL_STRUCT: {
    TRAVERSE(ast->structure.id);
    TRAVERSE(ast->structure.fields);
  } break;
  case FL_AST_DECL_STRUCT_FIELD: {
    TRAVERSE(ast->field.id);
    TRAVERSE(ast->field.type);
  } break;
  default: {}
  }
}