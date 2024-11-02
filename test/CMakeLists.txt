cmake_minimum_required(VERSION 3.10)

include(GoogleTest)

file(GLOB_RECURSE TMPSF_TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/test_*.cc")

# ##############################################################################
# MAKE TARGETS
# ##############################################################################

add_custom_target(build-tests COMMAND ${CMAKE_CTEST_COMMAND} --show-only)

# ##############################################################################
# "make XYZ_test"
# ##############################################################################
message(STATUS "TMPSF_TEST_SOURCES: ${TMPSF_TEST_SOURCES}")
foreach(tmpsf_test_source ${TMPSF_TEST_SOURCES})
  # Create a human readable name.
  get_filename_component(tmpsf_test_filename ${tmpsf_test_source} NAME)
  string(REPLACE ".cc" "" tmpsf_test_name ${tmpsf_test_filename})

  # Add the test target separately and as part of "make check-tests".
  # add_executable(${tmpsf_test_name} EXCLUDE_FROM_ALL)
  add_executable(${tmpsf_test_name} ${tmpsf_test_source})
  # add_backward(${tmpsf_test_name})
  add_dependencies(build-tests ${tmpsf_test_name})

  gtest_discover_tests(
    ${tmpsf_test_name}
    EXTRA_ARGS
      --gtest_output=xml:${CMAKE_BINARY_DIR}/test/${tmpsf_test_name}.xml
      --gtest_catch_exceptions=0 DISCOVERY_TIMEOUT 120
    PROPERTIES
    TIMEOUT 120)

  target_link_libraries(${tmpsf_test_name} tmpsf gtest)

  # Set test target properties and dependencies.
  set_target_properties(
    ${tmpsf_test_name}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
               COMMAND ${tmpsf_test_name})
endforeach()
