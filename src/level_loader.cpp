#include "level_loader.h"

Tile*** load_map(std::string level_path, Player* player, Camera* camera, Animation* tile_animations, int& map_width, int& map_height) {
  std::cout << "Loading map data from " << level_path << std::endl;
  //I don't remember how to do this well, so...

  std::ifstream map_data(level_path.c_str());
  std::string line;

  std::getline(map_data, line);
  std::stringstream header_stream(line);
  std::string header_cell;

  int count = 0;
  int player_x, player_y, camera_x, camera_y;

  while (std::getline(header_stream, header_cell, ',')) {
    switch (count) {
      case 0:
        map_width = atoi(header_cell.c_str());
        break;
      case 1:
        map_height = atoi(header_cell.c_str());
        break;
      case 2:
        player_x = atoi(header_cell.c_str());
        break;
      case 3:
        player_y = atoi(header_cell.c_str());
        break;
      case 4:
        camera_x = atoi(header_cell.c_str());
        break;
      case 5:
        camera_y = atoi(header_cell.c_str());
        break;
    };
    count++;
  }

  Tile*** game_map;
  game_map = new Tile**[map_width];
  for (int i=0; i < map_width; ++i)
    game_map[i] = new Tile*[map_height];

  int index_y = 0, index_x = 0;

  while (std::getline(map_data, line)) {
    std::stringstream line_stream(line);
    std::string cell;
    while (std::getline(line_stream, cell, ',')) {
      int map_tile = atoi(cell.c_str());
      if (map_tile == 4) {
        game_map[index_x][index_y] = new Tile(&tile_animations[map_tile-1], false, true);
      } else if (map_tile > 0) {
        game_map[index_x][index_y] = new Tile(&tile_animations[map_tile-1], false);
      } else {
        game_map[index_x][index_y] = new Tile();
      }
      
      index_x++;
      if (index_x == map_width) {
        index_x = 0;
        index_y++;
      }
    }
  }
  player->set_position(Point(player_x, player_y));
  camera->set_absolute(camera_x, camera_y);
  std::cout << "Finished loading " << map_width << "x" << map_height << std::endl;
  return game_map;
}