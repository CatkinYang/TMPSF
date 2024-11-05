#include "utils.h"
#include "gtest/gtest.h"
#include <iostream>

namespace TMPSF {

TEST(UTIL, BASIC_TEST) {
    std::cout << Utils::get_current_working_directory() << std::endl;
    std::cout << Utils::get_project_root() << std::endl;
}
} // namespace TMPSF

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); // Runs all the tests
}
