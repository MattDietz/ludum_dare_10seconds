#include "tile.h"

Tile::Tile(bool passable) {
  _animation = NULL;
  _passable = passable;
}

Tile::Tile(Animation* animation, bool passable) {
  _animation = animation;
  _passable = passable;
}

void Tile::set_animation(Animation* animation) {
  _animation = animation;
}

void Tile::draw(sf::RenderWindow* window, Point position) {
  if (_animation) {
    _animation->setPosition(position.x, position.y);
    window->draw(*_animation);
  }
}

void Tile::set_passable(bool state) {
  _passable = state;
}

bool Tile::passable() {
  return _passable;
}
