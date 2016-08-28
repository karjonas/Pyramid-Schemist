#include "level.h"

Level generate_level(size_t level_idx) {
  Level l;

  bool dir_flip = true;
  for (size_t i = 0; i < (level_idx + 1); i++) {
    Enemy e;
    e.pos_row = 0;
    e.pos_row_exact = 0;

    e.pos_col = 0;
    e.pos_col_exact = 0.5;

    e.direction_x = dir_flip ? 1 : -1;
    l.pyramid_enemies[i % 4].push_back(e);
    dir_flip = !dir_flip;
  }

  l.pyramids[1].blocks = create_block_layout();
  l.pyramids[2].blocks = create_block_layout();
  l.pyramids[3].blocks = create_block_layout();

  return l;
}
