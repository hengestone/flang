#include <stdio.h>
#include "flang.h"

// edit codes
// 2 invalid input
// 3 parse error
// 4 typesystem error
// 5 faltal error
// 6 unexpected error

int main(int argc, char** argv) {
  // TODO 0 means REPL
  if (argc == 1 || argc > 3) {
    fprintf(stderr, "Usage: flan file.fl [output.ir]\n");
    exit(2);
  }

  log_debug_level = 0;
  flang_init();

  // debug single file
  ast_t* root = fl_parse_main_file(argv[1]);
  // ast_t* root = fl_parse_file(argv[1]);
  if (ast_print_error(root)) {
    exit(3);
  }

  root = typesystem(root);
  if (ast_print_error(root)) {
    exit(4);
  }

  LLVMModuleRef module = fl_codegen(root, "test");

  if (argc == 3) {
    printf("export to ir %s\n", argv[2]);
    fl_to_ir(module, argv[2]);
  } else {
    printf("Running\n");
    // JIT has not been linked in.
    // cg_jit(module);
    fl_interpreter(module);
  }

  flang_exit();
  ast_delete(root);
  st_cleanup();

  return 0;
}
