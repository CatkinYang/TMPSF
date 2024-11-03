#include "task.h"
#include "shape.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace TMPSF {

Task::Task(int id, int clb, int dsp, int bram)
    : m_id(id), m_clb(clb), m_dsp(dsp), m_bram(bram) {
    m_children.clear();
    m_parent.clear();
    m_status = TaskStatus::WAITING;
}

Task::Task(int id, int clb, int dsp, int bram, int exec)
    : m_id(id), m_clb(clb), m_dsp(dsp), m_bram(bram), m_exectime(exec) {
    m_children.clear();
    m_parent.clear();
    m_status = TaskStatus::WAITING;
}

auto Task::sameHeightOpt() -> void {
    // key:height value: width
    std::unordered_map<int, std::vector<int>> map;
    std::vector<ShapeRef> new_shape_list;
    // 初始化map
    for (auto &s : m_shapelist) {
        auto hw = s->getHW();
        int h = hw.first, w = hw.second;
        map[h].push_back(w);
    }
    for (auto &[k, v] : map) {
        // 排序后第一个元素就是最小的
        sort(v.begin(), v.end());
        auto new_shape = std::make_shared<Shape>(v.front(), k);
        new_shape_list.push_back(new_shape);
    }
    m_shapelist = std::move(new_shape_list);
}

auto Task::printShape() -> void {
    int id = 0;
    for (auto &s : m_shapelist) {
        std::cout << id << " : ";
        std::cout << "Width: " << s->getHW().second
                  << " Height: " << s->getHW().first << std::endl;
    }
}
// auto Task::canConfig() -> bool {
//     for (const auto &p : m_parent) {
//         if (p->getStatus() != TaskStatus::ENDING) {
//             return false;
//         }
//     }
//     return true;
// }

} // namespace TMPSF
