#include "exit_arrow.h"

ExitArrow::ExitArrow(Rectangle rect) {
  _bounding_rect = rect;
}

void ExitArrow::set_frames(Animation* frames) {
  _frames = frames;
}

void ExitArrow::draw(sf::RenderWindow* window, Point camera)
{
  _frames->increment_frame();
  _frames->setPosition(_position.x - camera.x, _position.y - camera.y);
  window->draw(*_frames);
}

Point ExitArrow::position() {
  return _position;
}

void ExitArrow::set_pickup_sound(sf::Sound* sound) {
  _pickup_sound = sound;
}

void ExitArrow::set_position(float x, float y) {
  _position.x = x;
  _position.y = y;
}

Rectangle ExitArrow::get_bounding_rect() {
  float left = _position.x + _bounding_rect.left();
  float top = _position.y + _bounding_rect.top();
  float right = _position.x + _bounding_rect.right();
  float bottom = _position.y + _bounding_rect.bottom();
  return Rectangle(left, top, right, bottom);
}

void ExitArrow::perform_collision_action(Player* player, float& game_time, int& current_level) {
  _pickup_sound->play();
  current_level++;
  kill();
}
