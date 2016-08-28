#pragma once

enum BlockType { EMPTY, BLOCK, LADDER, BLOCK_IN, SNAKE, BLOCK_IN_SNAKE };

struct Block {
  BlockType type = EMPTY;
};
