#pragma once

#include <utility>

struct ALLEGRO_BITMAP;

enum class EnemyState { WALKING, FALLING, CLIMBING, DYING };

struct Enemy {
  int pos_row = 0;
  int pos_col = 0;

  double pos_row_exact = 0.5;
  double pos_col_exact = 0.5;

  ALLEGRO_BITMAP* image = nullptr;
  ALLEGRO_BITMAP* image1 = nullptr;

  int direction_x = -1; // -1=Left, 1=Right
  int direction_y =  0; // -1=down, 1=up, 0=still

  double speed = 2.5; // blocks per second
  double fall_speed = 4;

  bool move_next_side = false;

  EnemyState state = EnemyState::WALKING;
};
