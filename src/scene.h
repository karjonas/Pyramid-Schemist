#pragma once

#include "pyramid.h"

#include <utility>

struct ALLEGRO_KEYBOARD_STATE;

class Scene
{
public:
  Scene();

  void tick(ALLEGRO_KEYBOARD_STATE* kbd_state);
  void draw();

  void move_selector(int col_dt, int row_dt);

  Pyramid pyr;
  std::pair<int, int> selector_pos = {4,3};
  ALLEGRO_BITMAP *selector_img;
  double last_press_time = 0;
};
