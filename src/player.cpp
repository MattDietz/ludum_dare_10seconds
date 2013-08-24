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

Rectangle Player::get_bounding_rect() {
  float left = _position.x + _bounding_rect.left();
  float top = _position.y + _bounding_rect.top();
  float right = _position.x + _bounding_rect.right();
  float bottom = _position.y + _bounding_rect.bottom();
  return Rectangle(left, top, right, bottom);
}
