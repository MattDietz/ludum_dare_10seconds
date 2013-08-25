#include <iostream>
#include <list>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "config_lib/configfile.h"
#include "config_lib/configitem.h"

#include "animation.h"
#include "battery.h"
#include "collidable.h"
#include "camera.h"
#include "player.h"
#include "scoundrel_utils.h"
#include "tile.h"
#include "tile_helper.h"

//TODO: move away from all the globals. 
const float WALK = 0.25f;
const float MAX_WALK = 4.f;
const float WALK_STOP = 1.2f;
const int MOVE_DELTA = MAX_WALK;
const float JUMP_SPEED = -18.5f;

const float GRAVITY = 1.6f;
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
sf::Clock fps_clock, game_clock;
float game_time;
sf::SoundBuffer* sound_buffers;
sf::Sound* sounds;
Animation* animations, *tile_animations;

//Linked list would be much more efficient overall but I don't care right now
std::list<Entity*> game_entities;

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
  0,0,0,0,0,0,0,0,3,3,3,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,
  0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,
  3,0,0,3,3,3,1,3,3,3,3,3,3,3,3,3,3,3,3,3,
  1,4,4,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
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
        if (map_tile == 4) {
          //Spikes
          game_map[i][j] = new Tile(&tile_animations[map_tile-1], false, true);
        } else {
          game_map[i][j] = new Tile(&tile_animations[map_tile-1], false);
        }
      } else
        game_map[i][j] = new Tile();
    }
  }
  Battery* battery = new Battery(Rectangle(8, 8, 22, 28));
  battery->set_frames(&animations[4]);
  battery->set_position(100, 400);
  battery->set_pickup_sound(&sounds[1]);
  game_entities.push_back(battery);
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
  tile_animations = new Animation[NUM_ANIMATIONS];

  //Rock 1 Tile
  tile_animations[0].set_sprite_sheet(&tile_sheet);
  tile_animations[0].add_frame(sf::IntRect(32, 0, 32, 32));
  tile_animations[0].set_frame(0);

  //Rock 2 Tile
  tile_animations[1].set_sprite_sheet(&tile_sheet);
  tile_animations[1].add_frame(sf::IntRect(64, 0, 32, 32));
  tile_animations[1].set_frame(0);

  //Rock 1 Ground
  tile_animations[2].set_sprite_sheet(&tile_sheet);
  tile_animations[2].add_frame(sf::IntRect(224, 0, 32, 32));
  tile_animations[2].set_frame(0);

  //Brown Spikes
  tile_animations[3].set_sprite_sheet(&tile_sheet);
  tile_animations[3].add_frame(sf::IntRect(160, 0, 32, 32));
  tile_animations[3].set_frame(0);

  //Player walk right
  animations[0].set_sprite_sheet(&tile_sheet);
  animations[0].add_frame(sf::IntRect(0, 32, 32, 32));
  animations[0].add_frame(sf::IntRect(32, 32, 32, 32));
  animations[0].add_frame(sf::IntRect(64, 32, 32, 32));
  animations[0].add_frame(sf::IntRect(32, 32, 32, 32));
  animations[0].set_frame_time(5);
  animations[0].set_frame(0);

  //Player walk left
  animations[1].set_sprite_sheet(&tile_sheet);
  animations[1].add_frame(sf::IntRect(96, 32, 32, 32));
  animations[1].add_frame(sf::IntRect(128, 32, 32, 32));
  animations[1].add_frame(sf::IntRect(156, 32, 32, 32));
  animations[1].add_frame(sf::IntRect(128, 32, 32, 32));
  animations[1].set_frame_time(5);
  animations[1].set_frame(0);

  //Player stand left
  animations[2].set_sprite_sheet(&tile_sheet);
  animations[2].add_frame(sf::IntRect(96, 32, 32, 32));
  animations[2].set_frame(0);

  //Player stand right
  animations[3].set_sprite_sheet(&tile_sheet);
  animations[3].add_frame(sf::IntRect(0, 32, 32, 32));
  animations[3].set_frame(0);

  //Battery
  animations[4].set_sprite_sheet(&tile_sheet);
  animations[4].add_frame(sf::IntRect(352, 0, 32, 32));
  animations[4].add_frame(sf::IntRect(384, 0, 32, 32));
  animations[4].add_frame(sf::IntRect(416, 0, 32, 32));
  animations[4].add_frame(sf::IntRect(384, 0, 32, 32));
  animations[4].set_frame_time(5);
  animations[4].set_frame(0);

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

  game_font.loadFromFile("content/digital_tech.otf");
}

void init_audio() {
  sound_buffers = new sf::SoundBuffer[5];
  sounds = new sf::Sound[5];
  sound_buffers[0].loadFromFile("content/jump.wav");
  sounds[0].setBuffer(sound_buffers[0]);

  sound_buffers[1].loadFromFile("content/battery.wav");
  sounds[1].setBuffer(sound_buffers[1]);
}

void init_game()
{
  init_graphics();
  init_audio();

  player = new Player(&sprites[3], Point(300, 300), Rectangle(2, 4, 28, 30));
  player->set_walk_speed(WALK, MAX_WALK, WALK_STOP);
  player->set_movement(0, 0);
  player->set_fall_speed(GRAVITY, TERMINAL_VELOCITY);
  player->set_jump_speed(JUMP_SPEED);
  player->set_walk_frames(&animations[1], &animations[0]);
  player->set_stand_frames(&animations[2], &animations[3]);

  camera.set_absolute(0, 0);
  camera.set_window_size(WINDOW_WIDTH, WINDOW_HEIGHT);
  camera.set_window_snap(CAMERA_SNAP_X, CAMERA_SNAP_Y);
  camera.calculate_snap();

  game_time = 10.f;
}

void deinitialize_game(sf::RenderWindow* window) {
  delete window;
  delete player;
  delete[] textures;
  delete[] sprites;
  delete[] sound_buffers;
  delete[] sounds;
  delete[] animations;
  delete[] tile_animations;
  for (std::list<Entity *>::iterator it=game_entities.begin(); it != game_entities.end(); ++it) {
    delete *it;
  }
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

void player_collide_top(Point left, Point right, Point left_delta, Point right_delta) {
  Point player_tile_left = tile_helper.toTileCoords(left_delta);
  if (player_tile_left.y < 0) {
    sf::Vector2f player_movement = player->get_movement();
    player->set_movement(player_movement.x, 0);
    return;
  }

  Point player_tile_right = tile_helper.toTileCoords(right_delta);

  for (int i = (int)player_tile_left.x; i <= (int)player_tile_right.x; ++i) {
    int ptl = (int)player_tile_left.y;
    if (!game_map[i][ptl]->passable()) {
      sf::Vector2f player_movement = player->get_movement();
      Point tile_world = tile_helper.fromTileCoords(i, ptl); 
      float delta_y = float(left.y - tile_world.y - TILE_HEIGHT);
      if (delta_y == -1.f) {
        player->set_movement(player_movement.x, 0);
      } else if ((-delta_y) >= player_movement.y) {
        player->set_movement(player_movement.x, -delta_y);
      }
    }
  }
}

void player_collide_bottom(Point left, Point right, Point left_delta, Point right_delta) {
  Point player_tile_left = tile_helper.toTileCoords(left_delta);
  if (player_tile_left.y >= MAP_HEIGHT) {
    sf::Vector2f player_movement = player->get_movement();
    player->set_movement(player_movement.x, 0);
    return;
  }

  Point player_tile_right = tile_helper.toTileCoords(right_delta);
  for (int i = (int)player_tile_left.x; i <= (int)player_tile_right.x; ++i) {
    int ptl = (int) player_tile_left.y;
    if (!game_map[i][ptl]->passable()) {
      sf::Vector2f player_movement = player->get_movement();
      Point tile_world = tile_helper.fromTileCoords(i, ptl);
      float delta_y = float(tile_world.y - left.y);
      /*
       * Three states:
       *
       * Falling, but we'll land on this frame
       * Still Falling
       * Landed
       *
       * Falling, but land -> set_movement(player.move.x, diff(player, tile));
       * Still falling -> do nothing
       * landed -> unset(ENTITY_JUMPING) - landed counts as 1 px above the tile
       */
      if (delta_y <= 1.f) {
        player->unset_state(ENTITY_JUMPING);
        player->set_movement(player_movement.x, 0);
      } else if (delta_y > 1.f && delta_y <= player_movement.y) {
        player->unset_state(ENTITY_JUMPING);
        player->set_movement(player_movement.x, (delta_y - 1));
      } else {
        player->set_state(ENTITY_JUMPING);
      }
    }
    if (game_map[i][ptl]->is_deadly()) {
      std::cout << "Killed" << std::endl;
      player->kill();
      return;
    }
  }
}

void player_collide_left(Point top, Point bottom, Point top_delta, Point bottom_delta) {
  Point player_tile_top = tile_helper.toTileCoords(top_delta);
  if (player_tile_top.x < 0) {
    sf::Vector2f player_movement = player->get_movement();
    player->set_movement(0, player_movement.y);
    return;
  }

  Point player_tile_bottom = tile_helper.toTileCoords(bottom_delta);
  for (int i = (int)player_tile_top.y; i <= (int)player_tile_bottom.y; ++i) {
    int ptt = (int)player_tile_top.x;
    if (!game_map[ptt][i]->passable()) {
      sf::Vector2f player_movement = player->get_movement();
      Point tile_world = tile_helper.fromTileCoords(ptt, i);
      float delta_x = float(top.x - (tile_world.x + TILE_WIDTH));
      if (delta_x <= 0.f) {
        player->set_movement(0, player_movement.y);
      } else if (delta_x > 0.f && -delta_x >= player_movement.x) {
        player->set_movement(-delta_x + 1, player_movement.y);
      }
    }
  }
}

void player_collide_right(Point top, Point bottom, Point top_delta, Point bottom_delta) {
  Point player_tile_top = tile_helper.toTileCoords(top_delta);
  if (player_tile_top.x >= MAP_WIDTH) {
    sf::Vector2f player_movement = player->get_movement();
    player->set_movement(0, player_movement.y);
    return;
  }

  Point player_tile_bottom = tile_helper.toTileCoords(bottom_delta);
  for (int i = (int)player_tile_top.y; i <= (int)player_tile_bottom.y; ++i) {
    int ptt = (int)player_tile_top.x;
    if (!game_map[ptt][i]->passable()) {
      sf::Vector2f player_movement = player->get_movement();
      Point tile_world = tile_helper.fromTileCoords(ptt, i);
      float delta_x = float(tile_world.x - top.x);
      if (delta_x <= 0.f) {
        player->set_movement(0, player_movement.y);
      } else if (delta_x > 0.f && delta_x > player_movement.x) {
        player->set_movement(delta_x - 1, player_movement.y);
      }
      return;
    }
  }
}

void player_move_up(float delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_left_coords(player_rect.left(), player_rect.top());
  Point player_right_coords(player_rect.right(), player_rect.top());
  Point player_delta_left(player_left_coords.x, player_left_coords.y + delta);
  Point player_delta_right(player_right_coords.x, player_right_coords.y + delta);

  player_collide_top(player_left_coords, player_right_coords, player_delta_left, player_delta_right);
}

void player_move_down(float delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_left_coords(player_rect.left(), player_rect.bottom());
  Point player_right_coords(player_rect.right(), player_rect.bottom());
  Point player_delta_left(player_left_coords.x, player_left_coords.y + delta);
  Point player_delta_right(player_right_coords.x, player_right_coords.y + delta);

  player_collide_bottom(player_left_coords, player_right_coords, player_delta_left, player_delta_right);
}

void player_move_left(float delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_top_coords(player_rect.left(), player_rect.top());
  Point player_bottom_coords(player_rect.left(), player_rect.bottom());
  Point player_delta_top(player_top_coords.x + delta, player_top_coords.y);
  Point player_delta_bottom(player_bottom_coords.x + delta, player_bottom_coords.y);

  player_collide_left(player_top_coords, player_bottom_coords, player_delta_top, player_delta_bottom);
}

void player_move_right(float delta) {
  Rectangle player_rect = player->get_bounding_rect();

  Point player_top_coords(player_rect.right(), player_rect.top());
  Point player_bottom_coords(player_rect.right(), player_rect.bottom());
  Point player_delta_top(player_top_coords.x + delta, player_top_coords.y);
  Point player_delta_bottom(player_bottom_coords.x + delta, player_bottom_coords.y);

  player_collide_right(player_top_coords, player_bottom_coords, player_delta_top, player_delta_bottom);
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
        key_state.left_pressed = false;
      } 
      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        key_state.right_pressed = false;
      }
      if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        key_state.space_pressed = false;
        key_state.space_was_pressed = false;
      }
    }

    if (event.type == sf::Event::Closed)
      window->close();
  }

  if (key_state.left_pressed)
    player->walk_left();
  if (key_state.right_pressed)
    player->walk_right();

  if (key_state.space_pressed && !key_state.space_was_pressed) {
    sounds[0].play();
    player->jump();
    key_state.space_was_pressed = true;
  }

  if (!key_state.left_pressed && !key_state.right_pressed) {
    player->stop_walking();
  }
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

void draw_clock(sf::RenderWindow* window) {
  char frame_string[20];
  sprintf(frame_string, "%f\n", game_time);
  sf::Text test_text(frame_string, game_font);
  test_text.setPosition(WINDOW_WIDTH / 2, 20);
  window->draw(test_text);
}

void collide_objects() {
  Collidable temp;
  Rectangle player_rect = player->get_bounding_rect();
  for (std::list<Entity *>::iterator it=game_entities.begin(); it != game_entities.end(); ++it) {
    Rectangle other_rect = (*it)->get_bounding_rect();
    if (player_rect.intersects(&other_rect)) {
      Entity* game_entity = (*it);
      dynamic_cast<Collidable *>(game_entity)->perform_collision_action(player, game_time);
      if (!game_entity->is_alive()) {
        game_entities.erase(it);
      }
    }
  }
}

void game_loop(sf::RenderWindow* window) {
  int decay = 30;
  while (window->isOpen()) {
    Rectangle view = camera.get_view_rect();
    Point camera_pos = view.upper_left();

    handle_events(window);

    player_move();
    collide_objects();

    if (!player->is_alive())
      return;

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

    for (std::list<Entity *>::iterator it=game_entities.begin(); it != game_entities.end(); ++it) {
      (*it)->draw(window, camera_pos);
    }

    player->draw(window, camera_pos);

    if (show_fps)
      display_framerate(window);
    draw_clock(window);

    window->display();
    framerate = fps_clock.restart().asSeconds();
    game_time -= framerate;
    if (game_time <= 0.f) {
      return;
    }
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
