#pragma once
#include "fpga.h"
#include "task.h"
#include <set>
#include <unordered_set>
#include <vector>

namespace TMPSF {

// 任务的DAG图类
// 用于描述一个任务DAG信息
class TaskDAG {
  public:
    TaskDAG();

    // 随机初始化任务并生成一个DAG图
    auto init_from_file(const std::string &filename) -> void;
    auto init_from_json(const std::string &filename) -> void;

    // 随机生成一个任务
    auto init_random() -> void;
    auto task_info_init() -> TaskRef;
    auto task_info_init_custom(std::pair<int, int> clb, std::pair<int, int> dsp,
                               std::pair<int, int> bram,
                               std::pair<int, int> exec) -> TaskRef;
    static auto random_int_gen(int lower, int upper) -> int;
    static auto generate_normal_distribution(double mean, double stddev,
                                             int size) -> std::vector<double>;
    auto dag_print() -> void;
    auto edges_print() -> void;
    auto visualize_dag(const std::string &filename) -> void;

  private:
    int m_taskid;

    std::vector<std::vector<int>> dag_list;
    std::set<std::pair<int, int>> edges; // 只存储边的连接关系

    std::unordered_set<TaskRef> m_indgree;   // 存储该DAG所有入度为0的任务节点
    std::unordered_set<TaskRef> m_outdgree;  // 存储该DAG所有出度为0的任务节点
    std::unordered_map<int, TaskRef> m_task; // key:id, value:task
};

using TaskDAGRef = std::shared_ptr<TaskDAG>;

} // namespace TMPSF
