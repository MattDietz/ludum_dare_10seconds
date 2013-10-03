#include "game_map.h"

GameMap::GameMap(TileHelper* tile_helper) {
  _tile_helper = tile_helper;
  _game_map = NULL;
}

GameMap::~GameMap() {
  clear();
}

void GameMap::load_level(std::string proc_path, int level, Player* player, Camera* camera, std::map<int, Animation>* animation_map, sf::Sound* sounds, std::list<Entity *> &game_entities) {
  clear();
  std::string map_path;
  std::stringstream map_stream;
  map_stream << proc_path << "/level" << level;
  map_path = map_stream.str();
  _game_map = load_map(map_path, player, camera, animation_map, sounds, game_entities, _tile_helper, _width, _height);
}

Tile* GameMap::get_tile(int x, int y) {
  Tile* tile =  _game_map[x][y];
  return tile;
}

int GameMap::get_width() {
  return _width;
}

int GameMap::get_height() {
  return _height;
}

void GameMap::clear() {
  std::cout << "Cleaning up map data..." << std::endl;

  if (_game_map) {
    for (int i=0; i < _width; ++i) {
      for (int j=0; j < _height; ++j) {
        if (_game_map[i][j])
          delete _game_map[i][j];
      }
      delete _game_map[i];
    }
    delete[] _game_map;
  }
  _game_map = NULL;
}

void GameMap::draw(sf::RenderWindow* window, Point camera_pos, Point draw_start, Point draw_end) {
  for (int i=draw_start.x-1; i < draw_end.x+1; ++i) {
    Point row_coords = _tile_helper->fromTileCoords(0, i);
    if (i < 0 || i == _width)
      continue;
    for (int j=draw_start.y-1; j < draw_end.y+1; ++j) {
      if (j < 0 || j == _height)
        continue;
      _game_map[i][j]->draw(window, Point(i * _tile_helper->tile_width - camera_pos.x,
                                          j * _tile_helper->tile_height - camera_pos.y));
    }
  }
}
