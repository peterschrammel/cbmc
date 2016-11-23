SRC = c_test_case_generator.cpp expr2cleanc.cpp function_parameter_builder.cpp \
      function_return_builder.cpp c_test_file.cpp c_simple_test_case_generator.cpp \
      c_unity_test_case_generator.cpp

INCLUDES= -I ..

include ../config.inc
include ../common

CLEANFILES = test-c-gen$(LIBEXT)

all: test-c-gen$(LIBEXT)

###############################################################################

test-c-gen$(LIBEXT): $(OBJ)
	$(LINKLIB)
