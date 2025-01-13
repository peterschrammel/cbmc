if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(gcc_only -X gcc-only)
  set(gcc_only_string "-X;gcc-only;")
else()
  set(gcc_only "")
  set(gcc_only_string "")
endif()

add_test_pl_tests(
    "$<TARGET_FILE:cbmc> --validate-goto-model --validate-ssa-equation" -X smt-backend ${gcc_only}
)
