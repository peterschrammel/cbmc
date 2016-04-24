SRC = goto_diff_main.cpp goto_diff_parse_options.cpp \
      goto_diff_languages.cpp goto_diff_base.cpp syntactic_diff.cpp \
      unified_diff.cpp

OBJ += ../ansi-c/ansi-c$(LIBEXT) \
      ../linking/linking$(LIBEXT) \
      ../big-int/big-int$(LIBEXT) \
      ../goto-programs/goto-programs$(LIBEXT) \
      ../goto-symex/goto-symex$(LIBEXT) \
      ../pointer-analysis/value_set$(OBJEXT) \
      ../pointer-analysis/value_set_analysis_fi$(OBJEXT) \
      ../pointer-analysis/value_set_domain_fi$(OBJEXT) \
      ../pointer-analysis/value_set_fi$(OBJEXT) \
      ../pointer-analysis/value_set_dereference$(OBJEXT) \
      ../pointer-analysis/dereference_callback$(OBJEXT) \
      ../pointer-analysis/add_failed_symbols$(OBJEXT) \
      ../pointer-analysis/rewrite_index$(OBJEXT) \
      ../pointer-analysis/goto_program_dereference$(OBJEXT) \
      ../goto-instrument/full_slicer$(OBJEXT) \
      ../goto-instrument/nondet_static$(OBJEXT) \
      ../analyses/analyses$(LIBEXT) \
      ../langapi/langapi$(LIBEXT) \
      ../xmllang/xmllang$(LIBEXT) \
      ../assembler/assembler$(LIBEXT) \
      ../solvers/solvers$(LIBEXT) \
      ../util/util$(LIBEXT)

INCLUDES= -I ..

LIBS =

include ../config.inc
include ../common

CLEANFILES = goto-diff$(EXEEXT)

all: goto-diff$(EXEEXT)

ifneq ($(wildcard ../cpp/Makefile),)
  OBJ += ../cpp/cpp$(LIBEXT)
  CP_CXXFLAGS += -DHAVE_CPP
endif

ifneq ($(wildcard ../java_bytecode/Makefile),)
  OBJ += ../java_bytecode/java_bytecode$(LIBEXT)
  CP_CXXFLAGS += -DHAVE_JAVA_BYTECODE
  ifneq ($(LIBZIPLIB),)
    LIBS += -L $(LIBZIPLIB) -lzip -lz
  endif
endif

ifneq ($(wildcard ../specc/Makefile),)
  OBJ += ../specc/specc$(LIBEXT)
  CP_CXXFLAGS += -DHAVE_SPECC
endif

ifneq ($(wildcard ../php/Makefile),)
  OBJ += ../php/php$(LIBEXT)
  CP_CXXFLAGS += -DHAVE_PHP
endif

###############################################################################

goto-diff$(EXEEXT): $(OBJ)
	$(LINKBIN)

.PHONY: goto-diff-mac-signed

cbmc-mac-signed: goto-diff$(EXEEXT)
	strip goto-diff$(EXEEXT) ; codesign -v -s $(OSX_IDENTITY) goto-diff$(EXEEXT)

