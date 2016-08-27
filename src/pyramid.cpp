#include "pyramid.h"

#include "layout.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

Pyramid::Pyramid() {
  block_img = al_load_bitmap("img/block.png");
  ladder_img = al_load_bitmap("img/ladder.png");

  blocks = {{
              {{BLOCK, BLOCK, LADDER, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, LADDER, BLOCK, BLOCK}},
              {{EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, LADDER, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY}},
              {{EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, LADDER, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, LADDER, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, LADDER, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, LADDER, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}}
            }};
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
      }

    }
  }
}
