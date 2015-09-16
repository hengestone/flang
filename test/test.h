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
#define STR(val) #val

// last core typeid + 1
#define TEST_TYPEID 20

#define CHK_BODY(root)                                                         \
  ASSERT(root != 0, "root is not null");                                       \
  ASSERT(root->type == FL_AST_PROGRAM, "root is a program");                   \
  ASSERT(root->program.body->type == FL_AST_BLOCK, "program has body");        \
  ASSERT(root->program.body->block.nbody > 0, "body has statements");

#define CHK_GET_BODY(root, target)                                             \
  CHK_BODY(root);                                                              \
  target = root->program.body->block.body;

#define CHK_ERROR(root, target, msg)                                           \
  ASSERT(root != 0, "root is not null");                                       \
  ASSERT(root->type == FL_AST_PROGRAM, "root is a program");                   \
  target = root->program.body;                                                 \
  ASSERT(target->type == FL_AST_ERROR, "body is an error");                    \
  ASSERT(strcmp(target->err.str, msg) == 0, "error message match");

#define CHK_ERROR_RANGE(target, sc, sl, ec, el)                                \
  ASSERTE(target->token_start->start.column, sc, "%zu != %d", "start column"); \
  ASSERTE(target->token_start->start.line, sl, "%zu != %d", "start line");     \
  ASSERTE(target->token_end->end.column, ec, "%zu != %d", "end column");       \
  ASSERTE(target->token_end->end.line, el, "%zu != %d", "end line");

#define TEST_PARSER_OK(name, code, code_block)                                 \
  {                                                                            \
    fprintf(stderr, __FILE__ ":" STR(__LINE__) " @ " name "\n");               \
    ast_t* root = fl_parse_utf8(code);                                         \
    CHK_BODY(root);                                                            \
    ast_t** body = root->program.body->block.body;                             \
    code_block;                                                                \
    ts_exit();                                                                 \
    ast_delete(root);                                                          \
  }

#define TEST_PARSER_ERROR(name, code, msg, code_block)                         \
  {                                                                            \
    fprintf(stderr, __FILE__ ":" STR(__LINE__) " @ " name "\n");               \
    ast_t* root = fl_parse_utf8(code);                                         \
    ASSERT(root != 0, "root is not null");                                     \
    ASSERT(root->type == FL_AST_PROGRAM, "root is a program");                 \
    ast_t* err = root->program.body;                                           \
    ASSERT(err->type == FL_AST_ERROR, "body is an error");                     \
    ASSERT(strcmp(err->err.str, msg) == 0, "error message match");             \
    code_block;                                                                \
    ts_exit();                                                                 \
    ast_delete(root);                                                          \
  }

#define TEST_CODEGEN_OK(name, code, code_block)                                \
  {                                                                            \
    fprintf(stderr, __FILE__ ":" STR(__LINE__) " @ " name "\n");               \
    ast_t* root = fl_parse_utf8(code);                                         \
    CHK_BODY(root);                                                            \
    ast_t** body = root->program.body->block.body;                             \
    LLVMModuleRef module = fl_codegen(root, "test");                           \
    code_block;                                                                \
    ts_exit();                                                                 \
    ast_delete(root);                                                          \
  }
