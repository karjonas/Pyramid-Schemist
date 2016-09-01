#include "level.h"


pyr_arr_rc<BlockType> create_block_layout() {
  return  {{
              {{BLOCK, BLOCK, LADDER, BLOCK ,BLOCK,BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, LADDER, BLOCK, BLOCK}},
              {{EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY}},
              {{EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, LADDER, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, LADDER, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}}
            }};
}

pyr_arr_rc<BlockType> create_block_back() {
  return  {{
              {{BLOCK, BLOCK, LADDER, BLOCK, BLOCK,SNAKE,BLOCK, SNAKE, BLOCK, BLOCK, LADDER, BLOCK, BLOCK}},
              {{EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY}},
              {{EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, LADDER, BLOCK, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}},
              {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}}
            }};
}

pyr_arr_rc<BlockType> create_block_front() {
  return  {{
            {{BLOCK, BLOCK, LADDER, BLOCK, SNAKE, BLOCK, BLOCK, BLOCK, SNAKE, BLOCK, LADDER, BLOCK, BLOCK}},
            {{EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, LADDER, BLOCK, LADDER, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY}},
            {{EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY}},
            {{EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, LADDER, BLOCK, LADDER, BLOCK, BLOCK, EMPTY, EMPTY, EMPTY}},
            {{EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, BLOCK, BLOCK, BLOCK, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY}},
            {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, BLOCK, LADDER, BLOCK, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}},
            {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, LADDER, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}},
            {{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY}}
          }};
}

Level generate_level(size_t level_idx) {
  Level l;

  bool dir_flip = true;
  for (size_t i = 0; i < (level_idx + 1); i++) {
    Enemy e;
    e.pos_row = 0;
    e.pos_row_exact = 0;

    e.pos_col = (i % PYR_COLS);
    e.pos_col_exact = e.pos_col + 0.5;

    //e.speed += level_idx*0.15;

    e.speed = std::min(e.speed, 5.0);

    e.direction_x = dir_flip ? 1 : -1;
    e.pos_col_exact = e.pos_col + e.direction_x*0.5;

    e.pos_row = (i % 3);
    e.pos_row_exact = e.pos_row;

    l.pyramid_enemies[0].push_back(e);
    dir_flip = !dir_flip;
  }

  l.pyramids[0].blocks = create_block_front();
  l.pyramids[1].blocks = create_block_layout();
  l.pyramids[2].blocks = create_block_back();
  l.pyramids[3].blocks = create_block_layout();

  return l;
}
