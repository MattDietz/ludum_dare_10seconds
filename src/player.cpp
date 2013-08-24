#include "player.h"

Player::Player(sf::Sprite* sprite, Point position, Rectangle bounding_rect) : _sprite(sprite){
  _position = position;
  _bounding_rect = bounding_rect;
}


Player::~Player() {
  std::cout << "Cleaning up player" << std::endl;
}

void Player::draw(sf::RenderWindow* window, Point camera)
{
    _sprite->setPosition(_position.x - camera.x, _position.y - camera.y);
    window->draw(*_sprite);
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

bool Player::is_state_set(int state) {
  return _state & state;
}

void Player::set_state(int state) {
  _state |= state;
}

void Player::unset_state(int state) {
  _state &= ~state;
}

void Player::set_walk_speed(float walk, float max_walk) {
  _walk = abs(walk);
  _max_walk = abs(max_walk);
}

void Player::walk_left() {
  apply_movement(-_walk, 0);
  if (_movement.x < -_max_walk)
    set_movement(-_max_walk, _movement.y);
  set_state(ENTITY_WALKING);
}

void Player::walk_right() {
  apply_movement(_walk, 0);
  if (_movement.x >_max_walk)
    set_movement(_max_walk, _movement.y);
  set_state(ENTITY_WALKING);
}

void Player::stop_walking() {
  //Decelerate
  if (_state == ENTITY_WALKING) {
    if (_movement.x < 0) {
      apply_movement(_walk, 0);
      if (_movement.x > 0)
        set_movement(0, _movement.y);
    } else if (_movement.x > 0) {
      apply_movement(-_walk, 0);
      if (_movement.x < 0)
        set_movement(0, _movement.y);
    }

    if (round(_movement.x) == 0.f) {
      set_movement(0.f, _movement.y);
      unset_state(ENTITY_WALKING);
    }
  }
}

void Player::fall() {
  apply_movement(0, _fall_speed);
  if (_movement.y > _max_fall_speed)
    set_movement(_movement.x, _max_fall_speed);
}

void Player::jump(float magnitude) {
  if (!is_state_set(ENTITY_JUMPING)) {
    apply_movement(0.f, magnitude);
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
