#include "game_map.h"

GameMap::GameMap(TileHelper* tile_helper) {
  _tile_helper = tile_helper;
  _game_map = NULL;
}

GameMap::~GameMap() {
  clear();
}

void GameMap::load_level(int level, Player* player, Camera* camera, Animation* tile_animations) {
  clear();
  std::string map_path;
  std::stringstream map_stream;
  map_stream << "content/levels/level" << level;
  map_path = map_stream.str();
  _game_map = load_map(map_path, player, camera, tile_animations, _width, _height);
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
    for (int i=0; i < _height; ++i) {
      delete _game_map[i];
    }
    delete[] _game_map;
  }
}

void GameMap::draw(sf::RenderWindow* window, Point camera_pos, Point draw_start, Point draw_end) {
  for (int i=draw_start.y-1; i < draw_end.y+1; ++i) {
    Point row_coords = _tile_helper->fromTileCoords(0, i);
    if (i < 0 || i == _height)
      continue;
    for (int j=draw_start.x-1; j < draw_end.x+1; ++j) {
      if (j < 0 || j == _width)
        continue;
      _game_map[j][i]->draw(window, Point(j * _tile_helper->tile_width - camera_pos.x,
                                          i * _tile_helper->tile_height - camera_pos.y));
    }
  }
}
