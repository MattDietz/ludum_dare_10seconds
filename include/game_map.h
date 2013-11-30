#ifndef SCOUNDREL_GAMEMAP
#define SCOUNDREL_GAMEMAP

#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sstream>

#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

#include "animation.h"
#include "level_loader.h"
#include "player.h"
#include "tile.h"
#include "tile_helper.h"

class GameMap {
public:
  GameMap(TileHelper* tile_helper);
  ~GameMap();
  Tile* get_tile(int x, int y);
  void load_level(std::string proc_path, int level, Player* player, Camera* camera, std::map<int, Animation>* animation_map,
                  sf::Sound* sounds, std::list<Entity *>&game_entities);
  void draw(sf::RenderWindow* window, Point camera_pos, Point draw_start, Point draw_end, Player* player, std::list<Entity *> game_entities);
  void clear();
  int get_width();
  int get_height();
private:
  int get_light(int x, int y, Entity* entity);
  int _width, _height;
  TileHelper* _tile_helper;
  Tile*** _game_map;
};

#endif
