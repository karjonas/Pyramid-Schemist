#pragma once

#include "pyramid.h"
#include "enemy.h"

#include <array>
#include <vector>

struct Level
{
  std::array<Pyramid, 4> pyramids;
  std::array<std::vector<Enemy>, 4> pyramid_enemies;
};

Level generate_level(size_t level_idx);
