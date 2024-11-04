#include "task_manager.h"
#include "gtest/gtest.h"

namespace TMPSF {
TEST(JSON, BASIC_TEST1) {
    auto tm = new TaskManager();
    tm->init_from_json("/Users/catkin/Study/TMPSF/src/info/taskinfo/task.json");
    EXPECT_EQ(tm->task_num, 3);
    EXPECT_EQ(tm->getTask().at(1)->getBram(), 80);
}
} // namespace TMPSF

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); // Runs all the tests
}
