#pragma once

#include "pyramid.h"
#include "enemy.h"

#include <utility>
#include <vector>

struct ALLEGRO_KEYBOARD_STATE;

class Scene
{
public:
  Scene();

  void tick(ALLEGRO_KEYBOARD_STATE* kbd_state);
  void draw();

  void move_selector(int col_dt, int row_dt);
  void update_enemies(double dt);

  Pyramid pyr;
  std::pair<int, int> selector_pos = {4,3};
  ALLEGRO_BITMAP *selector_img;

  double last_press_time = 0;
  double last_update = 0;

  std::vector<Enemy> enemies;

  ALLEGRO_BITMAP *enemy_img0;
  ALLEGRO_BITMAP *enemy_img1;
  ALLEGRO_BITMAP *enemy_img2;

  double last_hole_time = 0.0;
};
