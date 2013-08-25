#ifndef SCOUNDREL_LEVEL_LOADER
#define SCOUNDREL_LEVEL_LOADER

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include "animation.h"
#include "camera.h"
#include "player.h"
#include "tile.h"


Tile*** load_map(std::string level_path, Player* player, Camera* camera,
              Animation* tile_animations, int& map_width, int& map_height);

#endif
