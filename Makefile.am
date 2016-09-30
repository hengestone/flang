ACLOCAL_AMFLAGS = -I m4

COMPILER_SOURCES = src/flang.c \
                src/debug.c \
                ext/array.c \
                ext/hash.c \
                ext/pool.c


AM_CPPFLAGS = -I$(top_srcdir)/ext \
              -I$(top_srcdir)/src

include_HEADERS = flang/flang.h \
  flang/debug.h \
  flang/libast.h \
  flang/libparser.h \
  flang/libcg.h \
  flang/libts.h


bin_PROGRAMS = flangc
flangc_SOURCES = src/main.c
flangc_SOURCES += $(COMPILER_SOURCES)

flangc_LDFLAGS = -lstdc++ -L/usr/local/lib '-lstringc -lm'
flangc_LDADD = libfl_codegen.a libfl_ts.a libfl_parser.a libfl_ast.a $(LIBS)
flangc_DEPENDENCIES = libfl_codegen.a libfl_ts.a libfl_parser.a libfl_ast.a


SOURCES_TEST = test/tasks.c \
               test/unit/hash-test.c \
               test/unit/typesystem-test.c \
               test/unit/parser-literals.c \
               test/unit/parser-expressions-test.c \
               test/unit/parser-variables-test.c \
               test/unit/parser-functions-test.c \
               test/unit/parser-types-test.c \
               test/unit/parser-if-test.c \
               test/unit/parser-loop-test.c \
               test/unit/parser-scopes-test.c \
               test/unit/codegen-expressions-test.c \
               test/unit/codegen-functions-test.c \
               test/codegen/codegen-tests.c

TESTS = tests
check_PROGRAMS = tests

tests_CFLAGS = -O1
tests_LDFLAGS = -O1 -lstdc++ -lrt -L/usr/local/lib '-lstringc'
tests_LDADD = libfl_codegen.a libfl_ts.a libfl_parser.a libfl_ast.a $(LIBS)
tests_DEPENDENCIES = libfl_codegen.a libfl_ts.a libfl_parser.a libfl_ast.a

tests_SOURCES = test/run-tests.c
#tests_SOURCES = test/run-benchmarks.c
tests_SOURCES += $(SOURCES_TEST)
tests_SOURCES += $(COMPILER_SOURCES)


noinst_LIBRARIES = libfl_ast.a libfl_parser.a libfl_ts.a libfl_codegen.a

libfl_ast_a_SOURCES = src/libast/ast-mk.c \
                        src/libast/ast-modify.c \
                        src/libast/ast-alloc.c \
                        src/libast/ast-traverse.c \
                        src/libast/ast-dump.c \
                        src/libast/ast-query.c \
                        src/libast/ast-reduce.c \
                        src/libast/ast-error.c \
                        src/libast/ast-search.c \
                        src/libast/ast-static.c \
                        src/libast/ast-expand.c \
                        src/libast/ast-reverse.c \
                        src/libast/ast-scope.c \
                        src/libast/ast.c

libfl_parser_a_SOURCES = src/libparser/grammar/parser.c \
                        src/libparser/grammar/tokens.c \
                        src/libparser/libparser.c

libfl_ts_a_SOURCES = src/libts/type-utils.c \
                        src/libts/typesystem-inference.c \
                        src/libts/typesystem-promotion.c \
                        src/libts/typesystem-pass.c \
                        src/libts/type-dump.c \
                        src/libts/type-check.c \
                        src/libts/typesystem-alloc.c \
                        src/libts/typesystem-casting.c \
                        src/libts/typesystem.c \
                        src/libts/typesystem-register.c \
                        src/libts/implement.c

libfl_ts_a_LIBADD = libfl_ast.a

libfl_codegen_a_SOURCES = src/libcg/c_codegen.c src/libcg/lua_codegen.c
