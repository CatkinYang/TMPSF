#include "tmpsf.h"
#include "shape.h"
#include <iostream>
#include <memory>
#include <ostream>
#include <thread>

namespace TMPSF {

auto TMPSF::judgeShapeValid(std::tuple<int, int, int> &has,
                            std::tuple<int, int, int> &need) -> bool {
    if (std::get<0>(has) < std::get<0>(need) ||
        std::get<1>(has) < std::get<1>(need) ||
        std::get<2>(has) < std::get<2>(need)) {
        return false;
    }
    return true;
}

auto TMPSF::check(FPGARef fp, TaskRef task, int shape_x, int shape_y) -> bool {
    int fpga_width = fp->getGraph()[0].size();
    int fpga_height = fp->getGraph().size();
    auto need = task->getResources();
    bool res = true;
    for (int i = 0; i < fpga_width; i++) {
        if (i + shape_x > fpga_width)
            break;
        for (int j = 0; j < fpga_height; j++) {
            if (j + shape_y > fpga_height)
                break;
            auto has = fp->getResources(i, j, shape_x, shape_y);
            if (!judgeShapeValid(has, need)) {
                res = false;
                break;
            }
        }
    }
    // std::cout << "Task " << task->getId() << " check result is: " << res
    //           << std::endl;
    return res;
}

// 1. 对于每个任务，枚举所有可能的宽度 0 - max_width
// 2. 在每个宽度下，从最大高度开始判断当前形状是否能满足要求
//    要求：该形状摆放在FPGA的任意位置，都可以满足资源需求
auto TMPSF::processTask(int id, TaskRef task, int max_width, int max_height)
    -> void {
    for (int w = 0; w < max_width; w++) {
        int h = max_height;
        bool move_flag = true;
        move_flag = check(m_fp, task, w, h);
        if (move_flag == false)
            continue;
        else {
            while (move_flag) {
                h = h - 1;
                move_flag = check(m_fp, task, w, h);
                if (move_flag == false) {
                    h = h + 1;
                    break;
                }
            }
        }
        double d_h = h, d_w = w;
        if (d_w / d_h <= 1.5) {
            auto s = std::make_shared<Shape>(w, h);
            std::cout << "Task: " << id << " ----- add to shape list -----"
                      << " W: " << w << " H: " << h << std::endl;
            task->addToShapeList(s);
        }
    }
}

auto TMPSF::taskShapeGenerator() -> void {
    auto tasks = m_tm->getTask();
    const auto &fpga = m_fp->getGraph();
    int max_height = fpga.size();
    int max_width = fpga[0].size();
    std::vector<std::thread> threads;
    for (const auto &[id, task] : tasks) {
        std::cout << "Task " << id << " starts to generator shape" << std::endl;
        threads.emplace_back(std::bind(&TMPSF::processTask, this, id, task,
                                       max_width, max_height));
    }
    for (auto &th : threads) {
        th.join();
    }
    for (const auto &[id, task] : tasks) {
        task->sameHeightOpt();
    }
}

} // namespace TMPSF
