#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "animation.h"
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

  //Animation boilerplate
  void set_stand_frames(Animation* left, Animation* right);
  void set_walk_frames(Animation* left, Animation* right);

  //Player actions
  void jump();
  void move(Point delta);
  void fall();

  int get_state();
  bool is_state_set(int state);
  void set_state(int state);
  void unset_state(int state);
  void reset();

  //Player walk handlers
  void set_walk_speed(float walk, float max_walk, float walk_stop);
  void walk_left();
  void walk_right();
  void float_up();
  void float_down();
  void stop_floating();
  void stop_walking();
  int get_illumination();

  void set_jump_speed(float magnitude);
  void set_fall_speed(float fall, float max_fall);

  Rectangle get_bounding_rect();
  Point position();

private:
  int _state;
  sf::Sprite* _sprite;
  Rectangle _bounding_rect;
  float _walk, _max_walk, _walk_stop;
  float _fall_speed, _max_fall_speed;
  float _jump_speed;

  bool _facing_right;

  //Animations
  Animation* _stand_left, *_stand_right;
  Animation* _walk_left;
  Animation* _walk_right;
};

#endif
