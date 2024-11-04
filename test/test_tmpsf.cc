#include "fpga.h"
#include "task_manager.h"
#include "tmpsf.h"
#include "gtest/gtest.h"
#include <iostream>
#include <memory>

namespace TMPSF {

TEST(TMPSF, BASIC_TEST1) {
    auto tm = std::make_shared<TaskManager>();
    tm->init_from_json("/Users/catkin/Study/TMPSF/src/info/taskinfo/task.json");
    auto fpga = std::make_shared<FPGA>("vc707");
    fpga->initFromFile("vc707");

    auto tmpsf = std::make_unique<TMPSF>(tm, fpga);
    std::cout << "******************START********************" << std::endl;
    tmpsf->taskShapeGenerator();
    std::cout << "*******************END*********************" << std::endl;

    for (const auto &[id, task] : tm->getTask()) {
        std::cout << "Task " << id << std::endl;
        task->printShape();
    }
}

} // namespace TMPSF

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); // Runs all the tests
}
