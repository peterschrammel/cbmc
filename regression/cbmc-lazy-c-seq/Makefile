default: test

include ../../src/config.inc
include ../../src/common

ifeq ($(BUILD_ENV_),MSVC)
GCC_ONLY = -X gcc-only
else
GCC_ONLY =
endif

test:
	@../test.pl -e -p -c "../../../src/cbmc/cbmc --validate-goto-model --validate-ssa-equation" -X smt-backend $(GCC_ONLY)

tests.log: ../test.pl test

clean:
	find . -name '*.out' -execdir $(RM) '{}' \;
	$(RM) tests*.log
