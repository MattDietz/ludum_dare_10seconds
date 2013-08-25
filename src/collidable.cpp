#include "collidable.h"

Collidable::Collidable() {}
void Collidable::perform_collision_action(Player* player) {
  std::cout << "Base collide" << std::endl;  
}
