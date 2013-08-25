#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "entity.h"
#include "entity_states.h"

Entity::Entity():_is_alive(true) {
  std::cout << "Entity Constructor" << std::endl;
}

Entity::~Entity() {
  std::cout << "Deleting Entity" << std::endl;
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
