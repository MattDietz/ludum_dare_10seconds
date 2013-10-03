#include "level_loader.h"

Tile*** load_map(std::string level_path, Player* player, Camera* camera, std::map<int, Animation>* animation_map,
                 sf::Sound* sounds, std::list<Entity *>& game_entities, TileHelper* tile_helper,
                 int& map_width, int& map_height) {
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
      /* Let's reorganize this:
       *
       * All blocking tiles are 1-99
       * All deadly tiles are 100-199
       * All decorative tiles are 200-299
       * All game entities are 300+
       */

      if (map_tile >= 100 && map_tile <= 199)
        game_map[index_x][index_y] = new Tile(&(*animation_map)[map_tile], false, true);
      else if (map_tile == 304) {
        //Battery
        Battery* battery = new Battery(Rectangle(8, 8, 22, 28));
        battery->set_frames(&(*animation_map)[map_tile]);
        Point pos = tile_helper->fromTileCoords(index_x, index_y);
        battery->set_position(pos);
        battery->set_pickup_sound(&sounds[1]);
        game_entities.push_back(battery);
        game_map[index_x][index_y] = new Tile();
      } else if (map_tile == 305 || map_tile == 306) {
        ExitArrow* exit_arrow = new ExitArrow(Rectangle(0, 0, 32, 32));
        exit_arrow->set_frames(&(*animation_map)[map_tile]);
        Point pos = tile_helper->fromTileCoords(index_x, index_y);
        exit_arrow->set_position(pos);
        exit_arrow->set_pickup_sound(&sounds[1]);
        game_entities.push_back(exit_arrow);
        game_map[index_x][index_y] = new Tile();
      } else if (map_tile > 0 && map_tile <= 99) {
        game_map[index_x][index_y] = new Tile(&(*animation_map)[map_tile], false);
      }  else {
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
  return game_map;
}
