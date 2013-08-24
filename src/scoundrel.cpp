#include <iostream>
#include <vector>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "config_lib/configfile.h"
#include "config_lib/configitem.h"

#include "animation.h"
#include "camera.h"
#include "player.h"
#include "scoundrel_utils.h"
#include "tile.h"
#include "tile_helper.h"

//TODO: move away from all the globals. 
const int WALK = 1;
const int MAX_WALK = 4;
const int MOVE_DELTA = MAX_WALK;

const float GRAVITY = 3.f;
const float TERMINAL_VELOCITY = 15.f;
const int MAP_WIDTH = 20, MAP_HEIGHT = 20;
const int TILE_WIDTH = 32, TILE_HEIGHT = 32;
const int FRAMERATE_LIMIT = 60;
const int NUM_ANIMATIONS=20;

// Impleent a camera/view class
int WINDOW_WIDTH, WINDOW_HEIGHT;
const float CAMERA_SNAP_X = 0.2f, CAMERA_SNAP_Y = 0.15f;

Tile*** game_map; //OH GOD
sf::Sprite* sprites;
sf::Texture* textures;
sf::Texture tile_sheet;

Player* player;
KeyState key_state;
Camera camera;
sf::Font game_font;
sf::Clock fps_clock;
sf::SoundBuffer* sound_buffers;
sf::Sound* sounds;
Animation* animations;

int simple_map[20][20] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

float framerate = 0.f;
bool show_fps = false;
TileHelper tile_helper(TILE_WIDTH, TILE_HEIGHT);

void load_config() {
  configlib::configfile config("10second.conf");
  configlib::configitem<int> window_width(config, "main", "int window_width", "height=", 1024);
  configlib::configitem<int> window_height(config, "main", "int window_height", "width=", 768);
  config.read();
  WINDOW_WIDTH = window_width;
  WINDOW_HEIGHT = window_height;
}

sf::Texture load_image(std::string image_path) {
  sf::Texture tex;
  if (!tex.loadFromFile(image_path)) {
    std::cout << "Failed to load " << image_path << std::endl;
  }
  return tex;
}

void init_map()
{
  game_map = new Tile**[MAP_WIDTH];
  for (int i=0; i <MAP_WIDTH; ++i) {
    game_map[i] = new Tile*[MAP_HEIGHT];
    for (int j=0; j < MAP_HEIGHT; j++) {
      int map_tile = simple_map[j][i];
      if (map_tile > 0) {
        game_map[i][j] = new Tile(&animations[map_tile], false);
      } else
        game_map[i][j] = new Tile();
    }
  }
}

void unload_map() {
  for (int i=0; i <MAP_HEIGHT; ++i) {
    delete game_map[i];
  }
  delete[] game_map;
}

sf::RenderWindow* init_sfml() {
  sf::RenderWindow* game_window = new sf::RenderWindow();
  game_window->create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Scoundrel");
  return game_window;
}

void init_tile_animations() {
  tile_sheet = load_image("content/tile_sheet.png");
  animations = new Animation[NUM_ANIMATIONS];

  //Rock 1 Tile
  animations[0].set_sprite_sheet(&tile_sheet);
  animations[0].add_frame(sf::IntRect(32, 0, 32, 32));
  animations[0].set_frame(0);

  //Rock 2 Tile
  animations[1].set_sprite_sheet(&tile_sheet);
  animations[1].add_frame(sf::IntRect(64, 0, 32, 32));
  animations[1].set_frame(0);

  //Rock 1 Ground
  animations[2].set_sprite_sheet(&tile_sheet);
  animations[2].add_frame(sf::IntRect(224, 0, 32, 32));
  animations[2].set_frame(0);
}

void init_graphics() {
  sprites = new sf::Sprite[10];
  textures = new sf::Texture[10];
  textures[0] = load_image("content/grass_32.jpg");
  sprites[0].setTexture(textures[0]);

  textures[1] = load_image("content/dirt_32.png");
  sprites[1].setTexture(textures[1]);

  textures[2] = load_image("content/rocks_32.png");
  sprites[2].setTexture(textures[2]);

  textures[3] = load_image("content/player.png");
  sprites[3].setTexture(textures[3]);

  init_tile_animations();
}

void init_audio() {
  sound_buffers = new sf::SoundBuffer[5];
  sounds = new sf::Sound[5];
  sound_buffers[0].loadFromFile("content/pewpew.wav");
  sounds[0].setBuffer(sound_buffers[0]);
}

void init_game()
{
  init_graphics();
  init_audio();

  //TODO Make this go away
  player = new Player(&sprites[3], Point(300, 300), Rectangle(2, 4, 24, 30));
  player->set_walk_speed(WALK, MAX_WALK);
  player->set_movement(0, 0);
  player->set_fall_speed(GRAVITY, TERMINAL_VELOCITY);

  camera.set_absolute(0, 0);
  camera.set_window_size(WINDOW_WIDTH, WINDOW_HEIGHT);
  camera.set_window_snap(CAMERA_SNAP_X, CAMERA_SNAP_Y);
  camera.calculate_snap();
}

void deinitialize_game(sf::RenderWindow* window) {
  delete window;
  delete player;
  delete[] textures;
  delete[] sprites;
  delete[] sound_buffers;
  delete[] sounds;
  delete[] animations;
}

void check_and_move_camera() {
  Point player_coords = player->position();
  Camera::CAMERA_SNAP cam_snap_horz;
  Camera::CAMERA_SNAP cam_snap_vert;

  cam_snap_horz = camera.point_snap_horizontal(player_coords);
  cam_snap_vert = camera.point_snap_vertical(player_coords);

  if (cam_snap_horz == Camera::SNAP_LEFT) {
    camera.move(-MOVE_DELTA, 0);
  } else if (cam_snap_horz == Camera::SNAP_RIGHT) {
    camera.move(MOVE_DELTA, 0);
  }

  if (cam_snap_vert == Camera::SNAP_TOP) {
    camera.move(0, -MOVE_DELTA);
  } else if (cam_snap_vert == Camera::SNAP_BOTTOM) {
    camera.move(0, MOVE_DELTA);
  }
}

bool player_collide_vertical(Point left, Point right) {
  Point player_tile_left = tile_helper.toTileCoords(left);
  if (player_tile_left.y < 0 || player_tile_left.y >= MAP_HEIGHT)
    return true;

  Point player_tile_right = tile_helper.toTileCoords(right);

  for (int i = (int)player_tile_left.x; i <= (int)player_tile_right.x; ++i) {
    if (!game_map[i][int(player_tile_left.y)]->passable())
      return true;
  }
  return false;
}

bool player_collide_horizontal(Point top, Point bottom) {
  Point player_tile_top = tile_helper.toTileCoords(top);
  if (player_tile_top.x < 0 || player_tile_top.x >= MAP_WIDTH)
    return true;

  Point player_tile_bottom = tile_helper.toTileCoords(bottom);
  for (int i = (int)player_tile_top.y; i <= (int)player_tile_bottom.y; ++i) {
    if (!game_map[int(player_tile_top.x)][i]->passable()) {
      return true;
    }
  }
  return false;
}

void player_move_up(float delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_left_coords(player_rect.left(), player_rect.top());
  Point player_right_coords(player_rect.right(), player_rect.top());
  Point player_delta_left(player_left_coords.x, player_left_coords.y + delta);
  Point player_delta_right(player_right_coords.x, player_right_coords.y + delta);

  if (player_collide_vertical(player_delta_left, player_delta_right)) {
    sf::Vector2f move = player->get_movement();
    player->apply_movement(0, -move.y);
  }
  check_and_move_camera();
}

void player_move_down(float delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_left_coords(player_rect.left(), player_rect.bottom());
  Point player_right_coords(player_rect.right(), player_rect.bottom());
  Point player_delta_left(player_left_coords.x, player_left_coords.y + delta);
  Point player_delta_right(player_right_coords.x, player_right_coords.y + delta);

  if (player_collide_vertical(player_delta_left, player_delta_right)) {
    sf::Vector2f move = player->get_movement();
    player->apply_movement(0, -move.y);
    player->unset_state(ENTITY_JUMPING);
  }
  check_and_move_camera();
}

void player_move_left(float delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_top_coords(player_rect.left(), player_rect.top());
  Point player_bottom_coords(player_rect.left(), player_rect.bottom());
  Point player_delta_top(player_top_coords.x + delta, player_top_coords.y);
  Point player_delta_bottom(player_bottom_coords.x + delta, player_bottom_coords.y);

  if (player_collide_horizontal(player_delta_top, player_delta_bottom)) {
    sf::Vector2f move = player->get_movement();
    player->apply_movement(-move.x, 0);
  }
  check_and_move_camera();
}

void player_move_right(float delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_top_coords(player_rect.right(), player_rect.top());
  Point player_bottom_coords(player_rect.right(), player_rect.bottom());
  Point player_delta_top(player_top_coords.x + delta, player_top_coords.y);
  Point player_delta_bottom(player_bottom_coords.x + delta, player_bottom_coords.y);

  if (player_collide_horizontal(player_delta_top, player_delta_bottom)) {
    sf::Vector2f move = player->get_movement();
    player->apply_movement(-move.x, 0);
  }
  check_and_move_camera();
}

void player_move() {
  player->fall();
  sf::Vector2f player_move = player->get_movement();

  if (player_move.x > 0)
    player_move_right(player_move.x);
  else if (player_move.x < 0)
    player_move_left(player_move.x);

  if (player_move.y > 0)
    player_move_down(player_move.y);
  else if (player_move.y < 0)
    player_move_up(player_move.y);

  player_move = player->get_movement();
  if (player_move.x == 0.f && player_move.y == 0.f)
    player->set_state(ENTITY_STANDING);
  if (player_move.y != 0.f)
    player->set_state(ENTITY_JUMPING); //technically falling

  player->move(Point(player_move.x, player_move.y));
}

void handle_events(sf::RenderWindow* window) {
  sf::Event event;
  while (window->pollEvent(event)) {
    if (event.type == sf::Event::KeyPressed) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        key_state.left_pressed = true;
      } 
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        key_state.right_pressed = true;
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        key_state.space_pressed = true;
      }

      switch(event.key.code) {
        case sf::Keyboard::Escape:
          window->close();
          break;
        case sf::Keyboard::F:
          show_fps = !show_fps;
          break;
      }
    } else if (event.type == sf::Event::KeyReleased) {
      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        key_state.left_was_pressed = key_state.left_pressed;
        key_state.left_pressed = false;
      } 
      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        key_state.right_was_pressed = key_state.right_pressed;
        key_state.right_pressed = false;
      }
      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        key_state.space_pressed = false;
      }
    }

    if (event.type == sf::Event::Closed)
      window->close();
  }

  if (key_state.left_pressed)
    player->walk_left();
  if (key_state.right_pressed)
    player->walk_right();

  if (key_state.space_pressed)
    player->jump(-30.0f);

  if (!key_state.left_pressed && !key_state.right_pressed) {
    player->stop_walking();
  }
}

void handle_ai() {

}

void draw_ui(sf::RenderWindow* window) {

}

void display_framerate(sf::RenderWindow* window) {
  //Super basic framerate calculator.
  //
  //TODO: Make a better one - http://gafferongames.com/game-physics/fix-your-timestep/
  char frame_string[5];
  int rate = (int)(1.0f / framerate);
  sprintf(frame_string, "%d\n", rate);
  sf::Text test_text(frame_string, game_font);
  test_text.setPosition(100, 20);
  window->draw(test_text);
}

void game_loop(sf::RenderWindow* window) {
  int decay = 30;
  while (window->isOpen()) {
    Rectangle view = camera.get_view_rect();
    Point camera_pos = view.upper_left();

    handle_events(window);
    handle_ai();

    //Replace later with all entity move
    player_move();

    check_and_move_camera();

    window->clear(sf::Color::Black);
    Point tile_start = tile_helper.toTileCoords(camera_pos);

    Point draw_start = tile_helper.toTileCoords(view.left(), view.top());
    Point draw_end = tile_helper.toTileCoords(view.right(), view.bottom());

    // Some basic attempts at tile clipping
    draw_start.x = draw_start.x < 0 ? 0 : draw_start.x;
    draw_start.y = draw_start.y < 0 ? 0 : draw_start.y;
    draw_start.x = draw_start.x > MAP_WIDTH ? MAP_WIDTH : draw_start.x;
    draw_start.y = draw_start.y > MAP_HEIGHT ? MAP_HEIGHT : draw_start.y;
    draw_end.x = draw_end.x > MAP_WIDTH ? MAP_WIDTH : draw_end.x;
    draw_end.y = draw_end.y > MAP_HEIGHT ? MAP_HEIGHT : draw_end.y;
    draw_end.x = draw_end.x < 0 ? 0 : draw_end.x;
    draw_end.y = draw_end.y < 0 ? 0 : draw_end.y;

    for (int i=draw_start.y-1; i < draw_end.y+1; ++i) {
      Point row_coords = tile_helper.fromTileCoords(0, i);
      if (i < 0 || i == MAP_WIDTH)
        continue;
      for (int j=draw_start.x-1; j < draw_end.x+1; ++j) {
        if (j < 0 || j == MAP_WIDTH)
          continue;
        game_map[j][i]->draw(window, Point(j * TILE_WIDTH - camera_pos.x, i * TILE_HEIGHT - camera_pos.y));
      }
    }
    player->draw(window, camera_pos);

    if (show_fps)
      display_framerate(window);

    draw_ui(window);
    window->display();
    framerate = fps_clock.restart().asSeconds();
  }
}


int main(int argc, char ** argv)
{
  load_config();
  sf::RenderWindow* window = init_sfml();
  window->setFramerateLimit(FRAMERATE_LIMIT);
  window->setVerticalSyncEnabled(true);
  init_game();
  init_map();
  game_loop(window);
  deinitialize_game(window);
  unload_map();
  return 0;
}
