#include "pyramid.h"

#include "layout.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

Pyramid::Pyramid() {
  block_img = al_load_bitmap("img/block.png");

  blocks = {{{{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
               EMPTY, EMPTY, EMPTY, EMPTY}},
             {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, EMPTY, EMPTY,
               EMPTY, EMPTY, EMPTY, EMPTY}},
             {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, EMPTY,
               EMPTY, EMPTY, EMPTY, EMPTY}},
             {{EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK,
               EMPTY, EMPTY, EMPTY, EMPTY}},
             {{EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK,
               BLOCK, EMPTY, EMPTY, EMPTY}},
             {{EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK,
               BLOCK, BLOCK, EMPTY, EMPTY}},
             {{EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK,
               BLOCK, BLOCK, BLOCK, EMPTY}},
             {{BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK,
               BLOCK, BLOCK, BLOCK, BLOCK}}}};
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
        al_draw_bitmap(block_img, offsetw + w * c, offseth + h * r, 0);
      }
    }
  }
}
