#ifndef SCOUNDREL_ENTITY
#define SCOUNDREL_ENTITY

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "animation.h"
#include "rect.h"
#include "scoundrel_utils.h"

class Entity {
public:
  Entity();
  virtual ~Entity();
  virtual void draw(sf::RenderWindow* window, Point camera_pos)=0;
  virtual void set_frames(Animation* frames);
  virtual Rectangle get_bounding_rect();
  virtual void kill();
  virtual void set_alive();
  virtual bool is_alive();
  virtual void apply_movement(float mag_x, float mag_y);
  virtual void set_position(Point position);
  virtual void set_position(float px, float py);
  virtual Point position();
  virtual void set_movement(float mag_x, float mag_y);
  virtual sf::Vector2f get_movement();
  virtual int get_illumination();

protected:
  sf::Vector2f _movement;
  Animation* _frames;
  Point _position;
  Point _size;
  bool _is_alive;
  Rectangle _bounding_rect;
};

#endif
