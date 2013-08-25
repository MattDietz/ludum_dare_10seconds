#include "collidable.h"

Collidable::Collidable() {}
void Collidable::perform_collision_action(Player* player, float& game_time) {
  std::cout << "Base collide" << std::endl;  
}
