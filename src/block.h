#pragma once

enum BlockType { EMPTY, BLOCK };

struct Block {
  BlockType type = EMPTY;
};
