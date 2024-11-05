#include "task_manager.h"
#include "tmpsf.h"
#include "gtest/gtest.h"

namespace TMPSF {

TEST(JSON, BASIC_TEST1) {
    auto tm = new TaskManager();
    tm->init_from_json("/Users/catkin/Study/TMPSF/src/info/taskinfo/task.json");
    EXPECT_EQ(tm->task_num, 6);
    EXPECT_EQ(tm->getTask().at(1)->getBram(), 80);
}

TEST(JSON, BASIC_TEST2) {
    auto tm = std::make_shared<TaskManager>();
    tm->init_from_json("/Users/catkin/Study/TMPSF/src/info/taskinfo/task.json");
    auto fpga = std::make_shared<FPGA>("vc707");
    fpga->initFromFile("vc707");
    auto tmpsf = std::make_unique<TMPSF>(tm, fpga);
    auto tasks = tm->getTask();
    for (const auto &[_, task] : tasks) {
        task->addToShapeList(1, 2);
        task->addToShapeList(2, 3);
    }
    tmpsf->taskFullInfoToJsonFile("task_shape");
}
} // namespace TMPSF
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); // Runs all the tests
}
