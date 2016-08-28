#include "pyramid.h"

#include "layout.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

Pyramid::Pyramid() {
  block_img = al_load_bitmap("img/block.png");
  block_in_img = al_load_bitmap("img/block_in.png");
  ladder_img = al_load_bitmap("img/ladder.png");
  snake_img = al_load_bitmap("img/snake_block.png");


  for (auto& row : inverted_block) {
      row.fill(0.0);
  }
}

void Pyramid::draw() {
  constexpr int w = 21;
  constexpr int h = 16;

  constexpr int totw = w * PYR_COLS;
  constexpr int toth = h * PYR_ROWS;

  constexpr int offsetw = (L_WIDTH - totw) / 2;
  constexpr int offseth = (L_HEIGHT - toth) / 2;

  for (int r = 0; r < PYR_ROWS; r++) {
    for (int c = 0; c < PYR_COLS; c++) {
      if (blocks[r][c] == BLOCK) {
        al_draw_bitmap(block_img, offsetw + w * c, L_HEIGHT - (offseth + h * r), 0);
      }
      else if (blocks[r][c] == LADDER) {
        al_draw_bitmap(ladder_img, offsetw + w * c, L_HEIGHT - (offseth + h * r), 0);
      } else if (blocks[r][c] == BLOCK_IN) {
        al_draw_bitmap(block_in_img, offsetw + w * c, L_HEIGHT - (offseth + h * r), 0);
      } else if (blocks[r][c] == SNAKE) {
        al_draw_bitmap(snake_img, offsetw + w * c, L_HEIGHT - (offseth + h * r), 0);
      }
    }
  }
}

BlockType Pyramid::get_block_at(int col, int row) {
  assert(col >= 0);
  assert(col < PYR_COLS);
  assert(row >= 0);
  assert(row < PYR_ROWS);

  return blocks[row][col];
}

bool Pyramid::invert_block_at(int col, int row, double time) {
  assert(col >= 0);
  assert(col < PYR_COLS);
  assert(row >= 0);
  assert(row < PYR_ROWS);

  auto b = blocks[row][col];

  if (b == BLOCK) {
    inverted_block[row][col] = time;
    blocks[row][col] = BLOCK_IN;
    return true;
  }

  return false;
}


void Pyramid::update(double dt) {
  for (int r = 0; r < PYR_ROWS; r++) {
    for (int c = 0; c < PYR_COLS; c++) {
        bool inverted = blocks[r][c] == BLOCK_IN;
        inverted_block[r][c] = std::max(0.0, inverted_block[r][c] - dt);
        if (inverted && inverted_block[r][c] <= 0.0) {
            blocks[r][c] = BLOCK;
        }
    }
  }
}
