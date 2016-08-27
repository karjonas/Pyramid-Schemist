#pragma once

#include <utility>

struct ALLEGRO_BITMAP;

struct Enemy {
  std::pair<int,int> pos = {0,0};
  ALLEGRO_BITMAP* image = nullptr;
};
