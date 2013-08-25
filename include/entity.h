#ifndef SCOUNDREL_ENTITY
#define SCOUNDREL_ENTITY

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "rect.h"
#include "scoundrel_utils.h"

class Entity {
public:
  Entity();
  virtual ~Entity();
  virtual Point position()=0;
  virtual void draw(sf::RenderWindow* window, Point camera_pos)=0;
  virtual Rectangle get_bounding_rect()=0;
  virtual void kill();
  virtual bool is_alive();
  virtual void apply_movement(float mag_x, float mag_y);
  virtual void set_movement(float mag_x, float mag_y);
  virtual sf::Vector2f get_movement();

protected:
  sf::Vector2f _movement;
  Point _position;
  Point _size;
  bool _is_alive;
};

#endif
