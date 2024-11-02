#include "fpga.h"
#include "gtest/gtest.h"
#include <memory>
#include <ostream>

namespace TMPSF {

TEST(FPGA_TEST, BASIC_TEST) {
    auto device = std::make_unique<FPGA>("vc707");
    EXPECT_EQ(device->getName(), "vc707");
    device->initFromFile("vc707");
    auto g = device->getGraph();
    EXPECT_EQ(g.size(), 350);
    EXPECT_EQ(g[0].size(), 146);
    std::cout << device->info() << std::endl;
    std::cout << "CLB: " << device->getClb() << " DSP: " << device->getDsp()
              << " BRAM: " << device->getBram() << std::endl;
}

} // namespace TMPSF

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS(); // Runs all the tests
}
