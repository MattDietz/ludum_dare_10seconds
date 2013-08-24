#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "entity.h"

Entity::Entity() {
  std::cout << "Entity Constructor" << std::endl;
}

Entity::~Entity() {
  std::cout << "Deleting Entity" << std::endl;
}

void Entity::draw(sf::RenderWindow* window) {
}
