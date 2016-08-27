#pragma once

#include "block.h"

#include <array>

#define PYR_ROWS 8
#define PYR_COLS 13

struct ALLEGRO_BITMAP;

class Pyramid {
public:
  Pyramid();

  void draw();

  ALLEGRO_BITMAP *block_img;
  std::array<std::array<BlockType, PYR_COLS>, PYR_ROWS> blocks;
};
