#pragma once

#include <memory>
namespace TMPSF {

// Task生成的形状
class Shape {
  public:
    Shape() = default;

  private:
    int m_width = 0;  // 宽度
    int m_height = 0; // 高度
    int m_x = 0;      // 左上角的x坐标
    int m_y = 0;      // 左上角的y坐标
};

using ShapeRef = std::shared_ptr<Shape>;

} // namespace TMPSF
