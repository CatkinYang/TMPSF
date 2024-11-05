#pragma once

#include "fpga.h"
#include "task_manager.h"
#include <iostream>
#include <map>
#include <shared_mutex>
#include <tuple>

namespace TMPSF {

class TMPSF {
  public:
    TMPSF(TaskManagerRef tm, FPGARef fp) : m_tm(tm), m_fp(fp) {}
    TMPSF(std::string &filename);

    // 每一个任务根据FPGA的信息生成对应的形状
    auto taskShapeGenerator() -> void;
    // 从Json文件中读取任务的形状
    auto tmpsfInitFromFullInfoJsonFile(const std::string &filename) -> void;

    // 将任务的形状信息输出为Json文件
    auto taskShapeInfoToJsonFIle(const std::string &filename) -> void;
    // 将任务的所有信息输出为Json文件
    auto taskFullInfoToJsonFile(const std::string &filename) -> void;

    auto processTask(int id, TaskRef task, int max_width, int max_height)
        -> void;

    auto judgeShapeValid(std::tuple<int, int, int> &has,
                         std::tuple<int, int, int> &need) -> bool;

    // 判断宽度为x，长度为y的形状是否能放在所有位置,并且满足任务对于资源的需求
    auto check(FPGARef fp, TaskRef task, int shape_x, int shape_y) -> bool;

  private:
    TaskManagerRef m_tm;
    FPGARef m_fp;
};

} // namespace TMPSF
