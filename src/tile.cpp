#include "tile.h"

Tile::Tile() {
  _passable = true;
}

Tile::Tile(sf::Sprite* sprite, int index_x, int index_y) {
  texture = sprite;
  index_x = index_x;
  index_y = index_y;
  _passable = true;
}

void Tile::draw(sf::RenderWindow* window, Point position) {
  texture->setPosition(position.x, position.y);
  window->draw(*texture);
}

void Tile::set_passable(bool state) {
  _passable = state;
}

bool Tile::passable() {
  return _passable;
}
