#pragma once

#include "task.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

using std::unordered_set;
namespace TMPSF {

// 任务管理类
// 管理批量输入的所有任务，建立图
class TaskManager {
  public:
    TaskManager() = default;
    // 根据输入的信息生成对应的Task假如到TaskSet中
    auto init() -> void;
    auto init_from_json(const std::string &filename) -> void;
    int task_num;
    auto getTask() { return m_tasks; }

  private:
    std::unordered_map<int, TaskRef> m_tasks;    // 存储id和Task之间的映射关系
    std::unordered_map<int, TaskRef> m_indegree; // 存储入度为0的任务
};

using TaskManagerRef = std::shared_ptr<TaskManager>;

} // namespace TMPSF
