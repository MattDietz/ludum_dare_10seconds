using namespace std;

#include "battery.h"

Battery::Battery(Rectangle rect) {
  _bounding_rect = rect;
}

void Battery::set_frames(Animation* frames) {
  _frames = frames;
}

void Battery::draw(sf::RenderWindow* window, Point camera)
{
  _frames->increment_frame();
  _frames->setPosition(_position.x - camera.x, _position.y - camera.y);
  window->draw(*_frames);
}

Point Battery::position() {
  return _position;
}

void Battery::set_pickup_sound(sf::Sound* sound) {
  _pickup_sound = sound;
}

void Battery::set_position(float x, float y) {
  _position.x = x;
  _position.y = y;
}

Rectangle Battery::get_bounding_rect() {
  float left = _position.x + _bounding_rect.left();
  float top = _position.y + _bounding_rect.top();
  float right = _position.x + _bounding_rect.right();
  float bottom = _position.y + _bounding_rect.bottom();
  return Rectangle(left, top, right, bottom);
}

void Battery::perform_collision_action(Player* player, float& game_time, int& current_level) {
  if (_is_alive) {
    game_time += 10.0f;
    _pickup_sound->play();
    kill();
  }
}
