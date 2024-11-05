#include "tmpsf.h"
#include "shape.h"
#include "json/include/json/json.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <thread>
#include <tuple>
#include <vector>

namespace TMPSF {

TMPSF::TMPSF(std::string &jsonfilename) {
    std::cout << "TMPSF begin construct" << std::endl;
    tmpsfInitFromFullInfoJsonFile(jsonfilename);
    std::cout << "TMPSF end construct" << std::endl;
}

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

            auto has = fp->getResources(i, j, shape_x, shape_y);
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

auto TMPSF::tmpsfInitFromFullInfoJsonFile(const std::string &filename) -> void {
    std::string path =
        "/Users/catkin/Study/TMPSF/pre_process/" + filename + ".json";
    std::ifstream jsonFile(path);
    if (!jsonFile.is_open()) {
        std::cerr << "Failed to open JSON file" << std::endl;
        return;
    }
    std::stringstream buffer; // 读取文件内容到字符串
    buffer << jsonFile.rdbuf();
    jsonFile.close(); // 关闭文件
    Json::Value root; // 解析 JSON 字符串
    Json::Reader reader;
    if (!reader.parse(buffer.str(), root)) {
        std::cerr << "Failed to parse JSON: "
                  << reader.getFormattedErrorMessages() << std::endl;
        return;
    }

    const Json::Value &tasks = root["tasks"];
    auto tm_tasks = m_tm->getTask();
    for (const auto &task : tasks) {
        int id = task["id"].asInt();

        int clb = task["clb"].asInt();
        int dsp = task["dsp"].asInt();
        int bram = task["bram"].asInt();
        int exectime = task["exectime"].asInt();

        auto t = std::make_shared<Task>(id, clb, dsp, bram, exectime);

        const Json::Value &children = task["children"];
        for (const auto &child : children) {
            t->addToChildren(child.asInt());
        }

        const Json::Value &parents = task["parents"];
        for (const auto &parent : parents) {
            t->addToChildren(parent.asInt());
        }
        std::vector<ShapeRef> shapeList;
        const Json::Value &shapes = task["shapes"];
        for (const auto &shape : shapes) {
            int h = shape["height"].asInt();
            int w = shape["width"].asInt();
            auto sp = std::make_shared<Shape>(h, w);
            shapeList.push_back(sp);
        }
        t->setShapeList(shapeList);

        tm_tasks[id] = t;
    }
}

auto TMPSF::taskShapeInfoToJsonFIle(const std::string &filename) -> void {
    Json::Value root;
    auto tasks = m_tm->getTask();
    for (const auto &[_, task] : tasks) {
        Json::Value taskJson;
        taskJson["id"] = task->getId();
        // 形状列表
        Json::Value shapesJson(Json::arrayValue); // JSON 数组
        for (const auto &shape :
             task->getShapes()) { // 假设 Task 类中有 getShapes()
                                  // 获取任务的所有形状
            Json::Value shapeJson;
            auto [height, width] = shape->getHW();
            shapeJson["height"] = height;
            shapeJson["width"] = width;
            shapesJson.append(shapeJson); // 添加形状到数组
        }

        taskJson["shapes"] = shapesJson;
        root["tasks"].append(taskJson); // 添加任务到根对象
    }

    // 将Json对象写入文件
    std::string path =
        "/Users/catkin/Study/TMPSF/pre_process/" + filename + ".json";
    std::ofstream file(path, std::ofstream::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    }

    Json::StreamWriterBuilder writer;
    file << Json::writeString(writer, root); // 格式化输出为 JSON 字符串
    file.close();
}

auto TMPSF::taskFullInfoToJsonFile(const std::string &filename) -> void {
    Json::Value root;
    auto tasks = m_tm->getTask();
    for (const auto &[_, task] : tasks) {
        Json::Value taskJson;
        taskJson["id"] = task->getId();
        taskJson["clb"] = task->getClb();
        taskJson["dsp"] = task->getDsp();
        taskJson["bram"] = task->getBram();
        taskJson["exectime"] = task->getExec();
        // children
        Json::Value childrenJson(Json::arrayValue); // JSON 数组
        for (const auto &c : task->getChildren()) {
            childrenJson.append(c);
        }
        taskJson["children"] = childrenJson;
        // parents
        Json::Value parentsJson(Json::arrayValue); // JSON 数组
        for (const auto &c : task->getParent()) {
            parentsJson.append(c);
        }
        taskJson["parents"] = childrenJson;
        // 形状列表
        Json::Value shapesJson(Json::arrayValue); // JSON 数组
        for (const auto &shape : task->getShapes()) {
            Json::Value shapeJson;
            auto [height, width] = shape->getHW();
            shapeJson["height"] = height;
            shapeJson["width"] = width;
            shapesJson.append(shapeJson); // 添加形状到数组
        }

        taskJson["shapes"] = shapesJson;
        root["tasks"].append(taskJson); // 添加任务到根对象
    }

    // 将Json对象写入文件
    std::string path =
        "/Users/catkin/Study/TMPSF/pre_process/" + filename + ".json";
    std::ofstream file(path, std::ofstream::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    }

    Json::StreamWriterBuilder writer;
    file << Json::writeString(writer, root); // 格式化输出为 JSON 字符串
    file.close();
}

} // namespace TMPSF
