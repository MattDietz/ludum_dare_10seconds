#ifndef SCOUNDREL_ENTITY
#define SCOUNDREL_ENTITY

#include <SFML/Window.hpp>

#include "rect.h"
#include "scoundrel_utils.h"

class Entity {
public:
  Entity();
  virtual ~Entity();
  virtual Point position()=0;
  virtual void draw(sf::RenderWindow* window);
  virtual Rectangle get_bounding_rect()=0;

protected:
  Point _position;
  Point _size;
};

#endif
