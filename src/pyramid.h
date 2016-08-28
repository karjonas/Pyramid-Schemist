#pragma once

#include "block.h"

#include <array>

#define PYR_ROWS 8
#define PYR_COLS 13

struct ALLEGRO_BITMAP;

template<typename T>
using pyr_arr_rc = std::array<std::array<T, PYR_COLS>, PYR_ROWS>;

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
  ALLEGRO_BITMAP *snake_img;

  pyr_arr_rc<BlockType> blocks;
  pyr_arr_rc<double> inverted_block;
};
