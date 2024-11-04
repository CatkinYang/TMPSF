#include "tmpsf.h"
#include "shape.h"
#include <iostream>
#include <memory>
#include <ostream>
#include <shared_mutex>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

namespace TMPSF {

auto TMPSF::judgeShapeValid(std::tuple<int, int, int> &has,
                            std::tuple<int, int, int> &need) -> bool {
    return std::get<0>(has) >= std::get<0>(need) &&
           std::get<1>(has) >= std::get<1>(need) &&
           std::get<2>(has) >= std::get<2>(need);
}

/* origin */
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

            // std::string key = std::to_string(i) + std::to_string(j) +
            //                   std::to_string(shape_x) +
            //                   std::to_string(shape_y);

            auto has = fp->getResources(i, j, shape_x, shape_y);

            // auto has = fp->getResources(i, j, shape_x, shape_y);
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
        std::cout << "Task: " << id << " optimization" << std::endl;
        task->printShape();
    }
}

// auto TMPSF::check(FPGARef fp, TaskRef task, int shape_x, int shape_y) -> bool
// {
//     int fpga_width = fp->getGraph()[0].size();
//     int fpga_height = fp->getGraph().size();
//     auto need = task->getResources();

//     for (int i = 0; i <= fpga_width - shape_x; ++i) {
//         for (int j = 0; j <= fpga_height - shape_y; ++j) {
//             auto has = getResourceSum(i, j, shape_y, shape_x);
//             if (!judgeShapeValid(has, need)) {
//                 return false;
//             }
//         }
//     }
//     return true;
// }

// auto TMPSF::processTask(int id, TaskRef task, int max_width, int max_height)
//     -> void {
//     for (int w = 1; w <= max_width; ++w) {
//         int h = max_height;
//         bool validShape = check(m_fp, task, w, h);

//         if (!validShape)
//             continue;

//         while (validShape) {
//             --h;
//             validShape = check(m_fp, task, w, h);
//             if (!validShape)
//                 ++h;
//         }

//         if (static_cast<double>(w) / h <= 1.5) {
//             auto shape = std::make_shared<Shape>(w, h);
//             task->addToShapeList(shape);
//             std::cout << "Task: " << id << " ----- Shape added: W: " << w
//                       << " H: " << h << std::endl;
//         }
//     }
// }

// auto TMPSF::taskShapeGenerator() -> void {
//     auto tasks = m_tm->getTask();
//     const auto &fpga = m_fp->getGraph();
//     int max_height = fpga.size();
//     int max_width = fpga[0].size();

//     std::vector<std::thread> threads;
//     for (const auto &[id, task] : tasks) {
//         threads.emplace_back(std::bind(&TMPSF::processTask, this, id, task,
//                                        max_width, max_height));
//     }
//     for (auto &th : threads) {
//         th.join();
//     }

//     for (const auto &[id, task] : tasks) {
//         task->sameHeightOpt();
//     }
// }
// auto TMPSF::calculatePrefixSum() -> void {
//     std::cout << "PrefixSum begin construct" << std::endl;
//     const auto &fpga = m_fp->getGraph();
//     int height = fpga.size();
//     int width = fpga[0].size();
//     prefixSum.resize(height + 1, std::vector<std::tuple<int, int, int>>(
//                                      width + 1, {0, 0, 0}));

//     for (int i = 1; i <= height; ++i) {
//         for (int j = 1; j <= width; ++j) {
//             int clb = (fpga[i - 1][j - 1] == 'c') ? 1 : 0;
//             int dsp = (fpga[i - 1][j - 1] == 'd') ? 1 : 0;
//             int bram = (fpga[i - 1][j - 1] == 'b') ? 1 : 0;
//             prefixSum[i][j] = {clb + std::get<0>(prefixSum[i - 1][j]) +
//                                    std::get<0>(prefixSum[i][j - 1]) -
//                                    std::get<0>(prefixSum[i - 1][j - 1]),
//                                dsp + std::get<1>(prefixSum[i - 1][j]) +
//                                    std::get<1>(prefixSum[i][j - 1]) -
//                                    std::get<1>(prefixSum[i - 1][j - 1]),
//                                bram + std::get<2>(prefixSum[i - 1][j]) +
//                                    std::get<2>(prefixSum[i][j - 1]) -
//                                    std::get<2>(prefixSum[i - 1][j - 1])};
//         }
//     }
//     std::cout << "PrefixSum end construct" << std::endl;
// }

// auto TMPSF::getResourceSum(int x, int y, int h, int w)
//     -> std::tuple<int, int, int> {
//     int x2 = x + h;
//     int y2 = y + w;
//     return {std::get<0>(prefixSum[x2][y2]) - std::get<0>(prefixSum[x][y2]) -
//                 std::get<0>(prefixSum[x2][y]) + std::get<0>(prefixSum[x][y]),
//             std::get<1>(prefixSum[x2][y2]) - std::get<1>(prefixSum[x][y2]) -
//                 std::get<1>(prefixSum[x2][y]) + std::get<1>(prefixSum[x][y]),
//             std::get<2>(prefixSum[x2][y2]) - std::get<2>(prefixSum[x][y2]) -
//                 std::get<2>(prefixSum[x2][y]) +
//                 std::get<2>(prefixSum[x][y])};
// }

} // namespace TMPSF
