#ifndef SCOUNDREL_TILE_HELPER
#define SCOUNDREL_TILE_HELPER

#include <cmath>
#include "scoundrel_utils.h"

struct TileHelper {
  int tile_width, tile_height;

  TileHelper(int tile_width, int tile_height):tile_width(tile_width), tile_height(tile_height) {}

  Point toTileCoords(float x, float y) {
    Point tile_coords;
    tile_coords.x = floor(x / tile_width);
    tile_coords.y = floor(y / tile_height);
    return tile_coords;
  }
  
  Point toTileCoords(Point pos) {
    Point tile_coords;
    tile_coords.x = floor(pos.x / tile_width);
    tile_coords.y = floor(pos.y / tile_height);
    return tile_coords;
  }
  
  Point fromTileCoords(Point pos) {
    Point world_coords;
    world_coords.x = pos.x * tile_width;
    world_coords.y = pos.y * tile_height;
    return world_coords;
  }
  
  Point fromTileCoords(int x, int y) {
    Point world_coords;
    world_coords.x = x * tile_width;
    world_coords.y = y * tile_height;
    return world_coords;
  }
};

#endif
