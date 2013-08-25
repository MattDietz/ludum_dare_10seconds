#ifndef SCOUNDREL_UTILS
#define SCOUNDREL_UTILS
struct Point {
  float x, y;
  Point() {}
  Point(float x, float y): x(x), y(y) {}
};

struct KeyState {
  bool left_pressed;
  bool right_pressed;
  bool up_pressed;
  bool down_pressed;
  bool space_pressed;

  bool left_was_pressed;
  bool right_was_pressed;
  bool up_was_pressed;
  bool down_was_pressed;
  bool space_was_pressed;
};

#endif

