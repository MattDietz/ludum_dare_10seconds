#include "rect.h"

Rectangle::Rectangle() {
}

Rectangle::Rectangle(float x1, float y1, float x2, float y2) {
  p1.x = x1;
  p1.y = y1;
  p2.x = x2;
  p2.y = y1;
  p3.x = x1;
  p3.y = y2;
  p4.x = x2;
  p4.y = y2;
}

void Rectangle::translate(float delta_x, float delta_y) {
  p1.x += delta_x;
  p2.x += delta_x;
  p3.x += delta_x;
  p4.x += delta_x;

  p1.y += delta_y;
  p2.y += delta_y;
  p3.y += delta_y;
  p4.y += delta_y;
}

float Rectangle::left() {
  return p1.x;
}

float Rectangle::right() {
  return p2.x;
}

float Rectangle::top() {
  return p1.y;
}

float Rectangle::bottom() {
  return p3.y;
}

Point Rectangle::upper_left() {
  return p1;
}

bool Rectangle::intersects(Rectangle* other) {
  //TODO: May need to return a more complex object later to designate which parts intersect
  //TODO: May also need to do actual line by line intersection as well.
  if ((p1.x >= other->left() && p1.x <= other->right() && p1.y >= other->top() && p1.y <= other->bottom()) ||
      (p2.x >= other->left() && p2.x <= other->right() && p2.y >= other->top() && p2.y <= other->bottom()) ||
      (p3.x >= other->left() && p3.x <= other->right() && p3.y >= other->top() && p3.y <= other->bottom()) ||
      (p4.x >= other->left() && p4.x <= other->right() && p4.y >= other->top() && p4.y <= other->bottom())) {
    return true;
  }
  return false;
}




