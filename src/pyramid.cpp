#include "pyramid.h"

#include "layout.h"
#include "paths.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

Pyramid::Pyramid()
{
    const auto block_img_path = get_full_data_path("img/block.png");
    const auto block_in_img_path = get_full_data_path("img/block_in.png");
    const auto ladder_img_path = get_full_data_path("img/ladder.png");
    const auto snake_img_path = get_full_data_path("img/snake_block.png");

    block_img = al_load_bitmap(block_img_path.c_str());
    block_in_img = al_load_bitmap(block_in_img_path.c_str());
    ladder_img = al_load_bitmap(ladder_img_path.c_str());
    snake_img = al_load_bitmap(snake_img_path.c_str());

    for (auto& row : inverted_block)
    {
        row.fill(0.0);
    }
}

int Pyramid::get_block_y(double row)
{
    constexpr int h = 16;
    constexpr int pyramid_height = PYR_ROWS * h;
    return pyramid_height - (h * (row + 1));
}

int Pyramid::get_block_x(double col)
{
    constexpr int w = 21;
    return w + w * col;
}

void Pyramid::draw()
{
    for (int r = 0; r < PYR_ROWS; r++)
    {
        for (int c = 0; c < PYR_COLS; c++)
        {
            const int block_y = get_block_y(r);
            const int block_x = get_block_x(c);
            if (blocks[r][c] == BLOCK)
            {
                al_draw_bitmap(block_img, block_x, block_y, 0);
            }
            else if (blocks[r][c] == LADDER)
            {
                al_draw_bitmap(ladder_img, block_x, block_y, 0);
            }
            else if (blocks[r][c] == BLOCK_IN)
            {
                al_draw_bitmap(block_in_img, block_x, block_y, 0);
            }
            else if (blocks[r][c] == BLOCK_IN_SNAKE)
            {
                al_draw_bitmap(block_in_img, block_x, block_y, 0);
                al_draw_bitmap(snake_img, block_x, block_y, 0);
            }
            else if (blocks[r][c] == SNAKE)
            {
                al_draw_bitmap(block_img, block_x, block_y, 0);
                al_draw_bitmap(snake_img, block_x, block_y, 0);
            }
        }
    }
}

BlockType Pyramid::get_block_at(int col, int row)
{
    assert(col >= 0);
    assert(col < PYR_COLS);
    assert(row >= 0);
    assert(row < PYR_ROWS);

    return blocks[row][col];
}

bool Pyramid::invert_block_at(int col, int row, double time)
{
    assert(col >= 0);
    assert(col < PYR_COLS);
    assert(row >= 0);
    assert(row < PYR_ROWS);

    auto b = blocks[row][col];

    if (b == BLOCK)
    {
        inverted_block[row][col] = time;
        blocks[row][col] = BLOCK_IN;
        return true;
    }
    else if (b == SNAKE)
    {
        inverted_block[row][col] = time;
        blocks[row][col] = BLOCK_IN_SNAKE;
        return true;
    }

    return false;
}

void Pyramid::update(double dt)
{
    for (int r = 0; r < PYR_ROWS; r++)
    {
        for (int c = 0; c < PYR_COLS; c++)
        {
            bool inverted = blocks[r][c] == BLOCK_IN;
            bool inverted_snake = blocks[r][c] == BLOCK_IN_SNAKE;
            inverted_block[r][c] = std::max(0.0, inverted_block[r][c] - dt);
            if (inverted && inverted_block[r][c] <= 0.0)
            {
                blocks[r][c] = BLOCK;
            }
            else if (inverted_snake && inverted_block[r][c] <= 0.0)
            {
                blocks[r][c] = SNAKE;
            }
        }
    }
}
