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

ast_t* ast_new() { return (ast_t*)calloc(1, sizeof(ast_t)); }
void ast_delete_list(ast_t** list) {
  size_t i = 0;
  ast_t* tmp;

  if (list) {
    while ((tmp = list[i++]) != 0) {
      ast_delete(tmp);
    }
  }

  free(list);
}

void ast_delete(ast_t* ast) {
  ast_delete_props(ast);
  free(ast);
}

void ast_delete_props(ast_t* ast) {

#define SAFE_DEL(test)                                                         \
  if (test) {                                                                  \
    ast_delete(test);                                                          \
    test = 0;                                                                  \
  }

#define SAFE_DEL_LIST(test)                                                    \
  if (test) {                                                                  \
    ast_delete_list(test);                                                     \
    test = 0;                                                                  \
  }

#define SAFE_DEL_STR(test)                                                     \
  if (test) {                                                                  \
    st_delete(&test);                                                          \
  }

  switch (ast->type) {
  case FL_AST_PROGRAM: {
    SAFE_DEL(ast->program.body);
    SAFE_DEL(ast->program.core);
    tk_tokens_delete(ast->program.tokens);
    SAFE_DEL_STR(ast->program.code);
  } break;
  case FL_AST_BLOCK: {
    SAFE_DEL_LIST(ast->block.body);
  } break;
  case FL_AST_LIST: {
    SAFE_DEL_LIST(ast->list.elements);
  } break;
  case FL_AST_EXPR_ASSIGNAMENT: {
    SAFE_DEL(ast->assignament.left);
    SAFE_DEL(ast->assignament.right);
  } break;
  case FL_AST_EXPR_BINOP: {
    SAFE_DEL(ast->binop.left);
    SAFE_DEL(ast->binop.right);
  } break;
  case FL_AST_EXPR_LUNARY: {
    SAFE_DEL(ast->lunary.element);
  } break;
  case FL_AST_EXPR_RUNARY: {
    SAFE_DEL(ast->runary.element);
  } break;
  case FL_AST_EXPR_CALL: {
    SAFE_DEL(ast->call.callee);
    SAFE_DEL(ast->call.arguments);
  } break;
  case FL_AST_EXPR_MEMBER: {
    SAFE_DEL(ast->member.left);
    SAFE_DEL(ast->member.property);
  } break;
  case FL_AST_LIT_STRING:
    SAFE_DEL_STR(ast->string.value);
    break;
  case FL_AST_LIT_IDENTIFIER:
    SAFE_DEL_STR(ast->identifier.string);
    break;
  case FL_AST_DTOR_VAR:
    SAFE_DEL(ast->var.id);
    SAFE_DEL(ast->var.type);
    break;
  case FL_AST_DECL_FUNCTION:
    SAFE_DEL(ast->func.id);
    SAFE_DEL_STR(ast->func.uid);
    SAFE_DEL(ast->func.ret_type);
    SAFE_DEL(ast->func.params);
    if (ast->func.body) {
      ast_delete(ast->func.body);
      ast->func.body = 0;
    }
    free(ast->func.body);
    break;
  case FL_AST_PARAMETER: {
    SAFE_DEL(ast->param.id);
    SAFE_DEL(ast->param.type);
  } break;
  case FL_AST_STMT_RETURN: {
    SAFE_DEL(ast->ret.argument);
  } break;
  case FL_AST_STMT_IF: {
    SAFE_DEL(ast->if_stmt.test);
    SAFE_DEL(ast->if_stmt.block);
    SAFE_DEL(ast->if_stmt.alternate);
  }
  case FL_AST_STMT_LOOP: {
    SAFE_DEL(ast->loop.init);
    SAFE_DEL(ast->loop.pre_cond);
    SAFE_DEL(ast->loop.update);
    SAFE_DEL(ast->loop.block);
    SAFE_DEL(ast->loop.post_cond);
  }
  case FL_AST_CAST: {
    SAFE_DEL(ast->cast.element);
  } break;
  case FL_AST_DECL_STRUCT: {
    SAFE_DEL(ast->structure.id);
    SAFE_DEL(ast->structure.fields);
  } break;
  case FL_AST_DECL_STRUCT_FIELD: {
    SAFE_DEL(ast->field.id);
    SAFE_DEL(ast->field.type);
  } break;
  default: {}
  }
}