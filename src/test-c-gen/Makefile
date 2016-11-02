SRC = c_test_case_generator.cpp

INCLUDES= -I ..

include ../config.inc
include ../common

CLEANFILES = test-c-gen$(LIBEXT)

all: test-c-gen$(LIBEXT)

###############################################################################

test-c-gen$(LIBEXT): $(OBJ)
	$(LINKLIB)
