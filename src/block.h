#pragma once

enum BlockType { EMPTY, BLOCK, LADDER, BLOCK_IN, SNAKE };

struct Block {
  BlockType type = EMPTY;
};
