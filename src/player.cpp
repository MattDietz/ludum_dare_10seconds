#include "player.h"

Player::Player(sf::Sprite* sprite, Point position, Rectangle bounding_rect) : _sprite(sprite){
  _position = position;
  _bounding_rect = bounding_rect;
}


Player::~Player() {
}

void Player::draw(sf::RenderWindow* window, Point camera)
{
  if (is_state_set(ENTITY_WALKING)) {
    if (_facing_right) {
      _walk_right->setPosition(_position.x - camera.x, _position.y - camera.y);
      window->draw(*_walk_right);
    } else {
      _walk_left->setPosition(_position.x - camera.x, _position.y - camera.y);
      window->draw(*_walk_left);
    }
  } else {
    if (_facing_right) {
      _stand_right->setPosition(_position.x - camera.x, _position.y - camera.y);
      window->draw(*_stand_right);
    } else {
      _stand_left->setPosition(_position.x - camera.x, _position.y - camera.y);
      window->draw(*_stand_left);
    }
  }
}

int Player::get_illumination() {
  return 120;
}

void Player::move(int x, int y) {
  _position.x += x;
  _position.y += y;
}

void Player::move(Point delta) {
  _position.x += delta.x;
  _position.y += delta.y;
}

Point Player::position() {
  return _position;
}

void Player::set_fall_speed(float fall, float max_fall) {
  _fall_speed = fall;
  _max_fall_speed = max_fall;
}

int Player::get_state() {
  return _state;
}

void Player::reset() {
  _state = 0;
  _movement.x = 0;
  _movement.y = 0;
}

bool Player::is_state_set(int state) {
  return _state & state;
}

void Player::set_state(int state) {
  _state |= state;
}

void Player::unset_state(int state) {
  _state &= ~state;
}

void Player::set_walk_frames(Animation* left, Animation* right) {
  _walk_left = left;
  _walk_right = right;
}

void Player::set_stand_frames(Animation* left, Animation* right) {
  _stand_left = left;
  _stand_right = right;
}

void Player::set_walk_speed(float walk, float max_walk, float walk_stop) {
  _walk = walk;
  _max_walk = max_walk;
  _walk_stop = walk_stop;
}

void Player::walk_left() {
  _walk_left->increment_frame();
  apply_movement(-_walk, 0);
  if (_movement.x < -_max_walk)
    set_movement(-_max_walk, _movement.y);
  _facing_right = false;
  set_state(ENTITY_WALKING);
}

void Player::walk_right() {
  _walk_right->increment_frame();
  apply_movement(_walk, 0);
  if (_movement.x >_max_walk)
    set_movement(_max_walk, _movement.y);
  _facing_right = true;
  set_state(ENTITY_WALKING);
}

void Player::float_up() {
  apply_movement(0, -_walk);
  if (_movement.y < -_max_walk)
    set_movement(_movement.x, -_max_walk);
}

void Player::float_down() {
  apply_movement(0, _walk);
  if (_movement.y >_max_walk)
    set_movement(_movement.x, _max_walk);
}

void Player::stop_floating() {
  set_movement(_movement.x, 0.f);
}

void Player::stop_walking() {
  //Decelerate
  if (_state == ENTITY_WALKING) {
    if (_movement.x < 0) {
      apply_movement(_walk_stop, 0);
      if (_movement.x > 0)
        set_movement(0, _movement.y);
    } else if (_movement.x > 0) {
      apply_movement(-_walk_stop, 0);
      if (_movement.x < 0)
        set_movement(0, _movement.y);
    }

    if (round(_movement.x) == 0.f) {
      set_movement(0.f, _movement.y);
      unset_state(ENTITY_WALKING);
    }
  }
}

void Player::set_jump_speed(float magnitude) {
  _jump_speed = magnitude;
}

void Player::fall() {
  apply_movement(0, _fall_speed);
  if (_movement.y > _max_fall_speed)
    set_movement(_movement.x, _max_fall_speed);
}

void Player::jump() {
  if (!is_state_set(ENTITY_JUMPING)) {
    apply_movement(0.f, _jump_speed);
    set_state(ENTITY_JUMPING);
  }
}

Rectangle Player::get_bounding_rect() {
  float left = _position.x + _bounding_rect.left();
  float top = _position.y + _bounding_rect.top();
  float right = _position.x + _bounding_rect.right();
  float bottom = _position.y + _bounding_rect.bottom();
  return Rectangle(left, top, right, bottom);
}
