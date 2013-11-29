#include "exit_arrow.h"

ExitArrow::ExitArrow(Rectangle rect) {
  _bounding_rect = rect;
}

void ExitArrow::draw(sf::RenderWindow* window, Point camera)
{
  _frames->increment_frame();
  _frames->setPosition(_position.x - camera.x, _position.y - camera.y);
  window->draw(*_frames);
}

void ExitArrow::set_pickup_sound(sf::Sound* sound) {
  _pickup_sound = sound;
}

void ExitArrow::perform_collision_action(Player* player, float& game_time, int& current_level) {
  _pickup_sound->play();
  current_level++;
  kill();
}

int ExitArrow::get_illumination() {
  return 180;
}
