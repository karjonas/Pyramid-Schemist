#pragma once

enum BlockType { EMPTY, BLOCK, LADDER, BLOCK_IN };

struct Block {
  BlockType type = EMPTY;
};
