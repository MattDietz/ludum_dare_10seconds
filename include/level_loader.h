#ifndef SCOUNDREL_LEVEL_LOADER
#define SCOUNDREL_LEVEL_LOADER

#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <fstream>

#include <SFML/Audio.hpp>

#include "animation.h"
#include "battery.h"
#include "camera.h"
#include "player.h"
#include "tile.h"
#include "tile_helper.h"


Tile*** load_map(std::string level_path, Player* player, Camera* camera, Animation* animations,
              Animation* tile_animations, sf::Sound* sounds, std::list<Entity *>& game_entities,
              TileHelper* tile_helper, int& map_width, int& map_height);

#endif
