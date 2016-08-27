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

  BlockType get_block_at(int col, int row);
  bool invert_block_at(int col, int row, double time);

  void update(double dt);

  ALLEGRO_BITMAP *block_img;
  ALLEGRO_BITMAP *block_in_img;
  ALLEGRO_BITMAP *ladder_img;
  std::array<std::array<BlockType, PYR_COLS>, PYR_ROWS> blocks;
  std::array<std::array<double, PYR_COLS>, PYR_ROWS> inverted_block;

};
