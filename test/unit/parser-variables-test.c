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

#include "../tasks.h"
#include "../test.h"

TASK_IMPL(parser_variables) {
  log_debug_level = 0;

  TEST_PARSER_OK("var decl 01", "var hello; hello = 1;", {
    ASSERT(body[0]->type == AST_DTOR_VAR, "type: AST_DTOR_VAR");
  });

  TEST_PARSER_OK("var decl 02", "var i8 hello;", {
    ASSERT(body[0]->type == AST_DTOR_VAR, "type: AST_DTOR_VAR");
    ASSERT(body[0]->var.type->type == AST_TYPE, "type.type: AST_TYPE");
    ASSERTE(body[0]->var.type->ty_id, TS_I8, "%zu == %d", "typeid i8 is TS_I8");
  });

  TEST_PARSER_OK("var decl 03", "var i8 hello = 1;",
                 { ASSERT(body[0]->type == AST_LIST, "type: AST_LIST"); });

  TEST_PARSER_OK("var decl 04", "var hello = \"world!\";",
                 { ASSERT(body[0]->type == AST_LIST, "type: AST_LIST"); });

  return 0;
}
