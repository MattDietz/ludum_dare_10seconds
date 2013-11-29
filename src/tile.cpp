#include "tile.h"

Tile::Tile(bool passable, bool deadly) {
  _animation = NULL;
  _passable = passable;
  _is_deadly = deadly;
}

Tile::Tile(Animation* animation, bool passable, bool deadly) {
  _animation = animation;
  _passable = passable;
  _is_deadly = deadly;
}

void Tile::set_animation(Animation* animation) {
  _animation = animation;
}

Animation* Tile::get_animation() {
  return _animation;
}

void Tile::draw(sf::RenderWindow* window, Point position, float brightness) {
  if (_animation) {
    _animation->setPosition(position.x, position.y);
    window->draw(*_animation);
  }
}

void Tile::set_passable(bool state) {
  _passable = state;
}

bool Tile::is_deadly() {
  return _is_deadly;
}

bool Tile::passable() {
  return _passable;
}
