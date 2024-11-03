#include "task_manager.h"
#include "json/include/json/json.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

namespace TMPSF {

auto TaskManager::init_from_json(const std::string &filename) -> void {
    std::ifstream jsonFile(filename);
    if (!jsonFile.is_open()) {
        std::cerr << "Failed to open JSON file" << std::endl;
        return;
    }

    // 读取文件内容到字符串
    std::stringstream buffer;
    buffer << jsonFile.rdbuf();

    // 关闭文件
    jsonFile.close();

    // 解析 JSON 字符串
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(buffer.str(), root)) {
        std::cerr << "Failed to parse JSON: "
                  << reader.getFormattedErrorMessages() << std::endl;
        return;
    }

    const Json::Value &tasks = root["tasks"];
    for (const auto &task : tasks) {
        int id = task["id"].asInt();
        int clb = task["clb"].asInt();
        int dsp = task["dsp"].asInt();
        int bram = task["bram"].asInt();
        int exectime = task["exectime"].asInt();

        auto t = std::make_shared<Task>(id, clb, dsp, bram, exectime);
        m_tasks[id] = t;

        const Json::Value &children = task["children"];
        for (const auto &child : children) {
            t->addToChildren(child.asInt());
        }
        const Json::Value &parents = task["parents"];
        for (const auto &parent : parents) {
            t->addToChildren(parent.asInt());
        }
        if (t->getParent().size() == 0) {
            m_indegree[id] = t;
        }
        task_num++;
    }

    return;
}

} // namespace TMPSF
