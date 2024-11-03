#include "task.h"

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

// auto Task::canConfig() -> bool {
//     for (const auto &p : m_parent) {
//         if (p->getStatus() != TaskStatus::ENDING) {
//             return false;
//         }
//     }
//     return true;
// }

} // namespace TMPSF
