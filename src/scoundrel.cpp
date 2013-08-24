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
const int MOVE_DELTA = 2;
const int MAP_WIDTH = 100, MAP_HEIGHT = 100;
const int TILE_WIDTH = 32, TILE_HEIGHT = 32;
const int FRAMERATE_LIMIT = 60;

// Impleent a camera/view class
int WINDOW_WIDTH, WINDOW_HEIGHT;
const float CAMERA_SNAP_X = 0.2f, CAMERA_SNAP_Y = 0.15f;

Tile*** game_map; //OH GOD
sf::Sprite* sprites;
sf::Texture* textures;

Player* player;
KeyState key_state;
Camera camera;
sf::Font game_font;
sf::Clock fps_clock;
sf::SoundBuffer* sound_buffers;
sf::Sound* sounds;

Animation fire_animation;
struct Fire {
  Point position;
};
Fire fires[100];

float framerate = 0.f;
bool show_fps = false;
TileHelper tile_helper(TILE_WIDTH, TILE_HEIGHT);

void load_config() {
  configlib::configfile config("scoundrel.conf");
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
      int rand_tile = rand() % 3;
      Tile* tile = new Tile(&sprites[rand_tile], i, j);
      if (rand_tile == 2)
        tile->set_passable(false);
      game_map[i][j] = tile;
    }
  }

  //Generate some fires
  for (int i = 0; i < 100; ++i) {
    int x = rand() % MAP_WIDTH * TILE_WIDTH;
    int y = rand() % MAP_HEIGHT * TILE_HEIGHT;
    fires[i].position = Point(x, y);
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

  textures[4] = load_image("content/fire_sheet.png");
  sprites[4].setTexture(textures[4]);
  fire_animation.set_sprite_sheet(&textures[4]);
  fire_animation.add_frame(sf::IntRect(0, 0, 31, 31));
  fire_animation.add_frame(sf::IntRect(33, 0, 63, 31));
  fire_animation.set_frame(0);

  game_font.loadFromFile("content/mensch.ttf");
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
  bool player_collision = false;
  for (int i = (int)player_tile_top.y; i <= (int)player_tile_bottom.y; ++i) {
    if (!game_map[int(player_tile_top.x)][i]->passable()) {
      player_collision = true;
    }
  }
  return player_collision;
}

void player_move_up(int delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_left_coords(player_rect.left(), player_rect.top());
  Point player_right_coords(player_rect.right(), player_rect.top());
  Point player_delta_left(player_left_coords.x, player_left_coords.y + delta);
  Point player_delta_right(player_right_coords.x, player_right_coords.y + delta);

  if (player_collide_vertical(player_delta_left, player_delta_right))
    return;

  player->move(0, delta);
  check_and_move_camera();
}

void player_move_down(int delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_left_coords(player_rect.left(), player_rect.bottom());
  Point player_right_coords(player_rect.right(), player_rect.bottom());
  Point player_delta_left(player_left_coords.x, player_left_coords.y + delta);
  Point player_delta_right(player_right_coords.x, player_right_coords.y + delta);

  if (player_collide_vertical(player_delta_left, player_delta_right))
    return;

  player->move(0, delta);
  check_and_move_camera();
}

void player_move_left(int delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_top_coords(player_rect.left(), player_rect.top());
  Point player_bottom_coords(player_rect.left(), player_rect.bottom());
  Point player_delta_top(player_top_coords.x + delta, player_top_coords.y);
  Point player_delta_bottom(player_bottom_coords.x + delta, player_bottom_coords.y);

  if (player_collide_horizontal(player_delta_top, player_delta_bottom))
    return;

  player->move(delta, 0);
  check_and_move_camera();
}

void player_move_right(int delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_top_coords(player_rect.right(), player_rect.top());
  Point player_bottom_coords(player_rect.right(), player_rect.bottom());
  Point player_delta_top(player_top_coords.x + delta, player_top_coords.y);
  Point player_delta_bottom(player_bottom_coords.x + delta, player_bottom_coords.y);

  if (player_collide_horizontal(player_delta_top, player_delta_bottom))
    return;

  player->move(delta, 0);
  check_and_move_camera();
}

void handle_events(sf::RenderWindow* window) {
  sf::Event event;

  while (window->pollEvent(event)) {
    if (event.type == sf::Event::KeyPressed) {
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        key_state.left_pressed = true;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        key_state.right_pressed = true;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        key_state.up_pressed = true;

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        key_state.down_pressed = true;

      switch(event.key.code) {
        case sf::Keyboard::Escape:
          window->close();
          break;
        case sf::Keyboard::F:
          show_fps = !show_fps;
          break;
      }
    } else if (event.type == sf::Event::KeyReleased) {
      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        key_state.left_pressed = false;

      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        key_state.right_pressed = false;

      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        key_state.up_pressed = false;

      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        key_state.down_pressed = false;
    }

    if (event.type == sf::Event::Closed)
      window->close();
  }

  if (key_state.left_pressed)
    player_move_left(-MOVE_DELTA);
  if (key_state.right_pressed)
    player_move_right(MOVE_DELTA);
  if (key_state.up_pressed)
    player_move_up(-MOVE_DELTA);
  if (key_state.down_pressed)
    player_move_down(MOVE_DELTA);
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
    for (int i = 0; i < 100; ++i) {
      Point fire = fires[i].position;
      fire.x -= camera_pos.x;
      fire.y -= camera_pos.y;
      fire_animation.setPosition(fire.x, fire.y);
      window->draw(fire_animation);
    }

    //TODO: nuke this shortly. Move into the animation class
    decay--;
    if (decay == 0) {
      int cur_frame = fire_animation.get_frame();
      if (cur_frame == 0)
        fire_animation.set_frame(1);
      else
        fire_animation.set_frame(0);
      decay = 30;
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
