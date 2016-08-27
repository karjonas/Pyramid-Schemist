#pragma once

#include <utility>

struct ALLEGRO_BITMAP;

enum class EnemyState { WALKING, FALLING, CLIMBING, DYING };

struct Enemy {
  std::pair<int,int> pos = {0,0};
  ALLEGRO_BITMAP* image = nullptr;

  int direction = -1; // -1=Left, 1=Right
  double speed = 0.5; // blocks per second

  std::pair<double, double> pos_exact = {0.0, 0.0};

  EnemyState state = EnemyState::WALKING;
};
