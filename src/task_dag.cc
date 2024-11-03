#include "task_dag.h"
#include "task.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <random>
#include <sstream>
#include <utility>
#include <vector>

namespace TMPSF {

TaskDAG::TaskDAG() {
    m_taskid = 0;
    m_indgree.clear();
    m_outdgree.clear();
}

auto TaskDAG::init_from_json(const std::string &filename) -> void {}

/*****************************************************************/

/*****************************************************************/
std::vector<int> processLineData(const std::string &line) {
    std::istringstream iss(line);
    int data1, data2, data3, data4, data5;
    std::vector<int> res;
    if (iss >> data1 >> data2 >> data3 >> data4 >> data5) {
        res = {data1, data2, data3, data4, data5};
        return res;
    } else {
        std::cerr << "无法解析行数据: " << line << std::endl;
        return res;
    }
}

auto TaskDAG::init_from_file(const std::string &filename) -> void {
    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            auto task_info = processLineData(line);
            int id = task_info[0];
            int clb = task_info[1];
            int dsp = task_info[2];
            int bram = task_info[3];
            int exec = task_info[4];
            auto TaskRef = std::make_shared<Task>(id, clb, dsp, bram, exec);
            m_task.insert({id, TaskRef});
        }
        inputFile.close();
    } else {
        std::cerr << "Can't open file: '" << filename << std::endl;
    }
}

auto TaskDAG::init_random() -> void {
    std::vector<double> alpha_vec = {0.5, 1.0, 1.5};
    std::vector<double> beta_vec = {0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0};

    auto n = random_int_gen(5, 25);                 // DAG节点数
    auto max_out = random_int_gen(1, 3);            // DAG最大出度
    double alpha = alpha_vec[random_int_gen(0, 2)]; // 控制形状的参数
    double beta = beta_vec[random_int_gen(0, 4)];   // 控制每层的宽度
    auto length = std::floor(std::sqrt(n) / alpha);
    double mean_value = static_cast<double>(n) / length;
    auto random_num = generate_normal_distribution(mean_value, beta, length);

    std::cout << "This DAG's -attributes: " << std::endl;
    std::cout << "Nodes  : " << n << std::endl;
    std::cout << "Max out: " << max_out << std::endl;
    std::cout << "Alpha  : " << alpha << std::endl;
    std::cout << "Beta   : " << beta << std::endl;
    std::cout << "Random num:" << std::endl;
    for (auto d : random_num) {
        std::cout << d << " ";
    }
    std::cout << std::endl;

    dag_list.clear();
    int generate_num = 0;

    // Initialize dag_list based on random_num
    for (double num : random_num) {
        dag_list.emplace_back();
        int tasks_count = static_cast<int>(std::ceil(num));
        for (int j = 0; j < tasks_count; ++j) {
            dag_list.back().push_back(j);
        }
        generate_num += tasks_count;
        std::cout << "This level has " << tasks_count << " node" << std::endl;
    }
    std::cout << "Init Task number is " << generate_num << std::endl;
    // Adjust to ensure total tasks equal to n
    if (generate_num != n) {
        if (generate_num < n) {
            for (int i = 0; i < n - generate_num; ++i) {
                int index = random_int_gen(0, length - 1);
                dag_list[index].push_back(dag_list[index].size());
            }
        } else {
            int i = 0;
            while (i < generate_num - n) {
                int index = random_int_gen(0, length - 1);
                if (dag_list[index].size() == 1) {
                    i = (i != 0) ? i - 1 : 0; // Don't decrement if i is 0
                } else {
                    dag_list[index].pop_back();
                }
                i++;
            }
        }
    }

    // Create edges
    std::vector<int> into_degree(n, 0);
    std::vector<int> out_degree(n, 0);
    edges.clear();
    int pred = 0;

    for (int i = 0; i < length - 1; ++i) {
        std::vector<int> sample_list(dag_list[i + 1].size());
        std::iota(sample_list.begin(), sample_list.end(),
                  0); // Fill with 0, 1, ..., size-1

        for (size_t j = 0; j < dag_list[i].size(); ++j) {
            int od = random_int_gen(0, max_out);
            od = std::min(static_cast<int>(dag_list[i + 1].size()), od);
            std::vector<int> bridges;

            for (int k = 0; k < od; ++k) {
                int index = random_int_gen(0, sample_list.size() - 1);
                bridges.push_back(sample_list[index]);
            }

            for (int k : bridges) {
                edges.insert(std::make_pair(dag_list[i][j] + pred,
                                            dag_list[i + 1][k] + pred +
                                                1)); // Connect edges
                into_degree[pred + k]++;             // Update in-degree
                out_degree[j + pred]++;
            }
        }
        pred += dag_list[i].size();
    }
    // Create start and exit nodes
    for (size_t node = 0; node < into_degree.size(); ++node) {
        if (into_degree[node] == 0) {
            edges.insert(std::make_pair(node, -1)); // Connect to start
            into_degree[node]++;
        }
    }

    for (size_t node = 0; node < out_degree.size(); ++node) {
        if (out_degree[node] == 0) {
            edges.insert(std::make_pair(node, -2)); // Connect to exit
            out_degree[node]++;
        }
    }
}

auto TaskDAG::task_info_init() -> TaskRef {
    auto clb = random_int_gen(2000, 3000);
    auto dsp = random_int_gen(0, 80);
    auto bram = random_int_gen(0, 80);
    auto exec = random_int_gen(5, 50);
    auto task = std::make_shared<Task>(m_taskid, clb, dsp, bram, exec);
    m_task.insert({m_taskid, task});
    m_taskid++;
    return task;
}

auto TaskDAG::task_info_init_custom(std::pair<int, int> clb,
                                    std::pair<int, int> dsp,
                                    std::pair<int, int> bram,
                                    std::pair<int, int> exec) -> TaskRef {
    auto clbn = random_int_gen(clb.first, clb.second);
    auto dspn = random_int_gen(dsp.first, dsp.second);
    auto bramn = random_int_gen(bram.first, bram.second);
    auto execn = random_int_gen(exec.first, exec.second);
    auto task = std::make_shared<Task>(m_taskid, clbn, dspn, bramn, execn);
    m_taskid++;
    return task;
}

auto TaskDAG::random_int_gen(int lower, int upper) -> int {
    std::random_device rd;  // 随机数生成器
    std::mt19937 gen(rd()); // 以随机设备作为种子的 Mersenne Twister 生成器
    std::uniform_int_distribution<> distr(lower, upper); //  均匀分布的整数分布
    return distr(gen);                                   //
}

// 生成一组符合正态分布（高斯分布）的随机数。
auto TaskDAG::generate_normal_distribution(double mean, double stddev, int size)
    -> std::vector<double> {
    std::vector<double> random_numbers(size);
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(mean, stddev);

    for (int i = 0; i < size; ++i) {
        random_numbers[i] = distribution(generator);
    }
    return random_numbers;
}

auto TaskDAG::dag_print() -> void {
    for (auto d : dag_list) {
        for (auto n : d) {
            std::cout << n << " ";
        }
        std::cout << std::endl;
    }
}

auto TaskDAG::edges_print() -> void {
    for (auto e : edges) {
        std::cout << e.first << " <-> " << e.second << std::endl;
    }
}

auto TaskDAG::visualize_dag(const std::string &filename) -> void {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename
                  << std::endl;
        return;
    }

    // Start DOT format
    outfile << "digraph G {" << std::endl;

    // Add nodes
    for (size_t i = 0; i < dag_list.size(); ++i) {
        for (int node : dag_list[i]) {
            outfile << "    " << node << " [label=\"Node " << node << "\"];"
                    << std::endl;
        }
    }

    // Add edges
    for (const auto &edge : edges) {
        outfile << "    " << edge.first << " -> " << edge.second << ";"
                << std::endl;
    }

    // End DOT format
    outfile << "}" << std::endl;

    outfile.close();
    std::cout << "DAG visualization written to " << filename << std::endl;
}

} // namespace TMPSF
