#ifndef SCOUNDREL_TILE
#define SCOUNDREL_TILE

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "animation.h"
#include "scoundrel_utils.h"


class Tile {
  /* Represents an individual tile in a map */
public:
  Tile(bool passable=true, bool deadly=false);
  Tile(Animation* animation, bool passable=true, bool deadly=false);
  void draw(sf::RenderWindow* window, Point position, float brightness);
  void set_animation(Animation* animation);
  Animation * get_animation();
  void set_passable(bool state);
  bool is_deadly();
  bool passable();

private:
  Animation* _animation;
  bool _passable;
  bool _is_deadly;
  //TODO: consider replacing with pointer back to the array later, or both
  int index_x, index_y; // reverse indices into the map
};

#endif
