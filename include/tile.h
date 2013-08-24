#ifndef SCOUNDREL_TILE
#define SCOUNDREL_TILE

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "scoundrel_utils.h"


class Tile {
  /* Represents an individual tile in a map */
public:
  Tile();
  Tile(sf::Sprite* sprite, int index_x, int index_y);
  void draw(sf::RenderWindow* window, Point position);
  void set_passable(bool state);
  bool passable();

private:
  sf::Sprite* texture;
  bool _passable;
  //TODO: consider replacing with pointer back to the array later, or both
  int index_x, index_y; // reverse indices into the map
};

#endif
