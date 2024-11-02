set(gtest_force_shared_crt
    ON
    CACHE BOOL "" FORCE)

set(FORT_ENABLE_TESTING
    OFF
    CACHE INTERNAL "")
add_subdirectory(googletest)
