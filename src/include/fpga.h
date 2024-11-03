#pragma once

#include <memory>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace TMPSF {
// FPGA 器件信息
// 根据配置文件生成对应的FPGA类
// 配置文件存储的FPGA资源分布情况
// clb dsp bram
// block:代表该区域没有资源可用
class FPGA {
  public:
    FPGA() = default;
    FPGA(string name) : m_name(name) {};

    // 根据输入的文件生成对应的FPGA资源情况
    auto initFromFile(const string &filename) -> void;

    // 输出FPGA的资源信息
    auto info() -> string;
    auto getName() { return m_name; }
    auto getGraph() { return m_graph; }
    auto getClb() { return m_clb; }
    auto getDsp() { return m_dsp; }
    auto getBram() { return m_bram; }
    auto getBlock() { return m_block; }

    // 获取以x,y为左上角顶点，w为宽度，h为高度的形状的资源情况
    auto getResources(int x, int y, int h, int w) -> std::tuple<int, int, int>;

  private:
    string m_name = "default name"; // FPGA名称
    vector<vector<char>> m_graph;   // FPGA资源分布
    vector<vector<bool>> m_used;    // FPGA资源使用
    int m_clb = 0;
    int m_dsp = 0;
    int m_bram = 0;
    int m_block = 0;
};

using FPGARef = std::shared_ptr<FPGA>;

} // namespace TMPSF
