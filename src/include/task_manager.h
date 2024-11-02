#pragma once

#include "task.h"
#include <unordered_set>

using std::unordered_set;
namespace TMPSF {

// 任务管理类
// 管理批量输入的所有任务，建立图
class TaskManager {
  public:
    TaskManager() = default;

    auto init() -> void;

  private:
    unordered_set<TaskRef> TaskSet;
};
} // namespace TMPSF
