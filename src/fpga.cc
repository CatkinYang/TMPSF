#include "include/fpga.h"
#include <fstream>
#include <sstream>
#include <string>

namespace TMPSF {

auto FPGA::initFromFile(const string &filename) -> void {
    std::string path =
        "/Users/catkin/Study/TMPSF/src/deviceinfo/" + filename + ".txt";
    std::ifstream inputFile(path);
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            std::vector<char> row;
            std::istringstream iss(line);
            char value;
            while (iss >> value) {
                if (value == 'c')
                    m_clb++;
                else if (value == 'd')
                    m_dsp++;
                else if (value == 'b')
                    m_bram++;
                else if (value == 'B')
                    m_block++;
                row.push_back(value);
            }
            m_graph.push_back(row);
        }
        inputFile.close();
    }
    int m = m_graph.size(), n = m_graph[0].size();
    m_used.resize(m);
    for (int i = 0; i < m; i++) {
        m_used[i].resize(n);
    }
}

auto FPGA::info() -> string {
    string info =
        "This FPGA: " + m_name + " has clb: " + std::to_string(m_clb) +
        " dsp: " + std::to_string(m_dsp) + " bram: " + std::to_string(m_bram);
    return info;
}

} // namespace TMPSF
