#pragma once

#include <memory>
#include <utility>
namespace TMPSF {

// Task生成的形状
class Shape {
  public:
    Shape() = default;
    Shape(int w, int h) : m_width(w), m_height(h) {};

    auto getHW() -> std::pair<int, int> { return {m_height, m_width}; }
    // auto getXY() -> std::pair<int, int> { return {m_x, m_y}; }

  private:
    int m_width = 0;  // 宽度
    int m_height = 0; // 高度
};

using ShapeRef = std::shared_ptr<Shape>;

} // namespace TMPSF
