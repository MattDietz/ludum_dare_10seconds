#include "scoundrel_utils.h"

#ifndef SCOUNDREL_RECTANGLE
#define SCOUNDREL_RECTANGLE

class Rectangle {
public:
  Rectangle();
  Rectangle(float x1, float y1, float x2, float y2);
  void translate(float delta_x, float delta_y);
  bool intersects(Rectangle* other_rect);
  float left();
  float right();
  float top();
  float bottom();
  Point upper_left();
private:
  // Wound clockwise
  Point p1, p2, p3, p4;
};

#endif
