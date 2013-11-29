#ifndef SCOUNDREL_BATTERY
#define SCOUNDREL_BATTERY

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include "animation.h"
#include "collidable.h"
#include "entity.h"
#include "rect.h"

class Battery : public Entity, public Collidable {
public:
  Battery(Rectangle bounding_rect);
  void draw(sf::RenderWindow * window, Point camera);
  void set_pickup_sound(sf::Sound* sound);
  void perform_collision_action(Player* player, float& game_time, int& current_level);
  int get_illumination();

private:
  sf::Sound* _pickup_sound;

};

#endif
