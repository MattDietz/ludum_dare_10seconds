#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "entity.h"
#include "entity_states.h"
#include "scoundrel_utils.h"

#ifndef SCOUNDREL_PLAYER
#define SCOUNDREL_PLAYER

class Player : public Entity {

public:
  ~Player();
  Player(sf::Sprite * sprite, Point position, Rectangle bounding_rect);
  void draw(sf::RenderWindow * window, Point camera);
  void move(int x, int y);
  void jump(float magnitude);
  void move(Point delta);

  int get_state();
  bool is_state_set(int state);
  void set_state(int state);
  void unset_state(int state);

  //Player walk handlers
  void set_walk_speed(float magnitude);
  void walk_left();
  void walk_right();
  void stop_walking();

  void set_fall_speed(float fall, float max_fall);
  void fall();

  Rectangle get_bounding_rect();
  Point position();

private:
  int _state;
  sf::Sprite* _sprite;
  Rectangle _bounding_rect;
  float _max_walk;
  float _fall_speed, _max_fall_speed;
};

#endif
