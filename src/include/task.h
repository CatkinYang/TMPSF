#pragma once
#include "shape.h"

#include <algorithm>
#include <memory>
#include <tuple>
#include <vector>

using std::shared_ptr;
using std::vector;

namespace TMPSF {

class Task;
using TaskRef = shared_ptr<Task>;

// 任务的状态：等待配置，配置中，运行中，结束
enum class TaskStatus { WAITING, CONFIGURING, RUNNING, ENDING };

// 任务类,包括
// 1. 任务的运行时间，配置时间
// 2. 任务对于资源的需求
// 3. 以及任务之间的依赖关系
class Task {
  public:
    Task(int id, int clb, int dsp, int bram);
    Task(int id, int clb, int dsp, int bram, int exec);

    auto getId() { return m_id; }
    auto getClb() { return m_clb; }
    auto getDsp() { return m_dsp; }
    auto getBram() { return m_bram; }
    auto getConf() { return m_conftime; }
    auto getExec() { return m_exectime; }
    auto getParent() { return m_parent; }
    auto getChildren() { return m_children; }
    auto getResources() -> std::tuple<int, int, int> {
        return {m_clb, m_dsp, m_bram};
    }
    auto getStatus() { return m_status; }
    auto getShapes() { return m_shapelist; }
    // auto canConfig() -> bool;
    auto addToParent(int id) { m_parent.push_back(id); }
    auto addToChildren(int id) { m_children.push_back(id); }
    auto addToShapeList(ShapeRef s) { m_shapelist.push_back(s); }
    auto addToShapeList(int w, int h) {
        auto s = std::make_shared<Shape>(w, h);
        m_shapelist.push_back(s);
    }
    auto setShapeList(std::vector<ShapeRef> sl) { m_shapelist = std::move(sl); }
    auto sameHeightOpt() -> void;
    auto printShape() -> void;

  private:
    // 任务id
    int m_id = 0;
    // 资源需求
    int m_clb = 0;
    int m_dsp = 0;
    int m_bram = 0;

    // 时间需求：conf:配置时间；exec:执行时间
    int m_conftime = 0;
    int m_exectime = 0;

    // 任务的相关状态
    // bool is_root = true; // 标识该任务是否有前序任务
    // bool is_end = false; // 判断该任务是否有后续任务
    TaskStatus m_status;

    // int m_indegree = 0;  // 入度
    // int m_outdegree = 0; // 出度

    // 任务之间的依赖关系
    // 该任务的所有父任务完成后，才能开始进行配置
    // 该任务完成之后，孩子任务才可以开始进行配置
    vector<int> m_children; // 存储该任务的孩子任务id
    vector<int> m_parent;   // 存储该任务的父任务id

    vector<ShapeRef> m_shapelist; // 任务的候选形状列表
};

} // namespace TMPSF
