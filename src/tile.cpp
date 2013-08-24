#include "tile.h"

Tile::Tile() {
  _passable = true;
}

Tile::Tile(Animation* animation) {
  _animation = animation;
}

void Tile::set_animation(Animation* animation) {
  _animation = animation;
}

void Tile::draw(sf::RenderWindow* window, Point position) {
  _animation->setPosition(position.x, position.y);
  window->draw(*_animation);
}

void Tile::set_passable(bool state) {
  _passable = state;
}

bool Tile::passable() {
  return _passable;
}
