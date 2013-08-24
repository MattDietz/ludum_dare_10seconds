#ifndef SCOUNDREL_TILE
#define SCOUNDREL_TILE

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "animation.h"
#include "scoundrel_utils.h"


class Tile {
  /* Represents an individual tile in a map */
public:
  Tile(bool passable=true);
  Tile(Animation* animation, bool passable=true);
  void draw(sf::RenderWindow* window, Point position);
  void set_animation(Animation* animation);
  void set_passable(bool state);
  bool passable();

private:
  Animation* _animation;
  bool _passable;
  //TODO: consider replacing with pointer back to the array later, or both
  int index_x, index_y; // reverse indices into the map
};

#endif
