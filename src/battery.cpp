using namespace std;

#include "battery.h"

Battery::Battery(Rectangle rect) {
  _bounding_rect = rect;
}

void Battery::draw(sf::RenderWindow* window, Point camera)
{
  _frames->increment_frame();
  _frames->setPosition(_position.x - camera.x, _position.y - camera.y);
  window->draw(*_frames);
}

void Battery::set_pickup_sound(sf::Sound* sound) {
  _pickup_sound = sound;
}

void Battery::perform_collision_action(Player* player, float& game_time, int& current_level) {
  game_time += 10.0f;
  _pickup_sound->play();
  kill();
}

int Battery::get_illumination() {
  return 255;
}
