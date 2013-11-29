#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "entity.h"
#include "entity_states.h"

Entity::Entity():_is_alive(true) {
}

Entity::~Entity() {
  std::cout << "Deleting Entity" << std::endl;
}

void Entity::set_frames(Animation* frames) {
  _frames = frames;
}

void Entity::apply_movement(float mag_x, float mag_y) {
  _movement.x += mag_x;
  _movement.y += mag_y;
}

void Entity::set_movement(float mag_x, float mag_y) {
  _movement.x = mag_x;
  _movement.y = mag_y;
}

sf::Vector2f Entity::get_movement() {
  return _movement;
}

void Entity::kill() {
  _is_alive = false;
}

bool Entity::is_alive() {
  return _is_alive;
}

void Entity::set_alive() {
  _is_alive = true;
}

void Entity::set_position(Point position) {
  _position = position;
}

void Entity::set_position(float px, float py) {
  _position.x = px;
  _position.y = py;
}

Point Entity::position() {
  return _position;
}

int Entity::get_illumination() {
  return 0;
}

Rectangle Entity::get_bounding_rect() {
  float left = _position.x + _bounding_rect.left();
  float top = _position.y + _bounding_rect.top();
  float right = _position.x + _bounding_rect.right();
  float bottom = _position.y + _bounding_rect.bottom();
  return Rectangle(left, top, right, bottom);
}
