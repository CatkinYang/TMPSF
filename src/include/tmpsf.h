#pragma once

#include "fpga.h"
#include "task_manager.h"
#include <iostream>
#include <map>
#include <tuple>

namespace TMPSF {

class TMPSF {
  public:
    TMPSF(TaskManagerRef tm, FPGARef fp) : m_tm(tm), m_fp(fp) {
        std::cout << "TMPSF begin construct" << std::endl;
        // calculatePrefixSum();
        std::cout << "TMPSF end construct" << std::endl;
    }

    // 每一个任务根据FPGA的信息生成对应的形状
    auto taskShapeGenerator() -> void;
    auto processTask(int id, TaskRef task, int max_width, int max_height)
        -> void;
    auto judgeShapeValid(std::tuple<int, int, int> &has,
                         std::tuple<int, int, int> &need) -> bool;

    // 判断宽度为x，长度为y的形状是否能放在所有位置,并且满足任务对于资源的需求
    auto check(FPGARef fp, TaskRef task, int shape_x, int shape_y) -> bool;

    // auto calculatePrefixSum() -> void;
    // auto getResourceSum(int x, int y, int h, int w)
    //     -> std::tuple<int, int, int>;

  private:
    TaskManagerRef m_tm;
    FPGARef m_fp;
    std::vector<std::vector<std::tuple<int, int, int>>> prefixSum;
};

} // namespace TMPSF
