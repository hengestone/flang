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
struct id_search {
  string* needle;
  fl_ast_t** list;
  size_t length;
};
typedef struct id_search id_search_t;

bool fl_ast_find_identifier(fl_ast_t* node, fl_ast_t* parent, size_t level,
                            void* userdata) {
  if (node->type == FL_AST_LIT_IDENTIFIER) {
    id_search_t* data = (id_search_t*)userdata;
    string* str = data->needle;
    if (st_cmp(str, node->identifier.string) == 0) {
      data->list[data->length++] = node;
    }
  }
  return true;
}

bool dtors_var_infer(fl_ast_t* node, fl_ast_t* parent, size_t level,
                     void* userdata) {
  switch (node->type) {
  case FL_AST_DTOR_VAR:
    if (node->var.type->ty.id == 0) {
      // search all ocurrences of this identifier
      id_search_t data;
      data.list = calloc(sizeof(fl_ast_t*), 100); // TODO resizable
      data.needle = node->var.id->identifier.string;
      data.length = 0;

      fl_ast_traverse(node->parent, fl_ast_find_identifier, 0, 0, (void*)&data);

      if (data.length) {
        // TODO REVIEW
        // right now if the first apparence is not a dtor -> undefined var
        size_t i = 0;
        for (; i < data.length; ++i) {
          fl_ast_t* fnod = data.list[i];

          // lhs of an expression
          if (fnod->parent->type == FL_AST_EXPR_ASSIGNAMENT &&
              fnod->parent->assignament.left == fnod) {
            // type is the right one
            size_t t = fl_ast_ret_type(fnod->parent->assignament.right);
            if (t) {
              node->var.type->ty.id = t;
              break;
            }
          }
        }
      }

      free(data.list);
    }
    break;
  }

  return true;
}

// return error
fl_ast_t* fl_pass_inference(fl_ast_t* node) {
  fl_ast_parent(node);
  // var x = 10; <- double
  // var x = ""; <- string*
  // var x = []; <- look usage
  // var x = [10]; <- array<double>

  // var i64 x = 10; <- cast 10 to i64
  fl_ast_traverse(node, dtors_var_infer, 0, 0, 0);
  return 0;
}
