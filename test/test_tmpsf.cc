#include "fpga.h"
#include "task_manager.h"
#include "tmpsf.h"
#include "gtest/gtest.h"

#include <iostream>
#include <memory>

namespace TMPSF {

TEST(TMPSF, T10_1) {
    auto tm = std::make_shared<TaskManager>();
    tm->init_from_json("t10_1.json");
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
    tmpsf->taskFullInfoToJsonFile("vc707_t10_1");
}

TEST(TMPSF, T10_2) {
    auto tm = std::make_shared<TaskManager>();
    tm->init_from_json("t10_2.json");
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
    tmpsf->taskFullInfoToJsonFile("vc707_t10_2");
}

TEST(TMPSF, T10_3) {
    auto tm = std::make_shared<TaskManager>();
    tm->init_from_json("t10_3.json");
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
    tmpsf->taskFullInfoToJsonFile("vc707_t10_3");
}

} // namespace TMPSF

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); // Runs all the tests
}
