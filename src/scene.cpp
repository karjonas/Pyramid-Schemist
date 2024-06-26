#include "scene.h"
#include "pyramid.h"
#include "layout.h"
#include "level.h"
#include "paths.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

#include <cmath>
#include <random>

#define HOLE_RETRY_TIME 1.0

Scene::Scene()
{
    const auto selector_img_path = get_full_data_path("img/selector.png");
    const auto selector_inv_img_path =
        get_full_data_path("img/selector_inactive.png");

    const auto enemy_img0_path = get_full_data_path("img/enemy0.png");
    const auto enemy_img01_path = get_full_data_path("img/enemy01.png");
    const auto enemy_img1_path = get_full_data_path("img/enemy1.png");
    const auto enemy_img11_path = get_full_data_path("img/enemy11.png");
    const auto enemy_img2_path = get_full_data_path("img/enemy2.png");
    const auto enemy_img21_path = get_full_data_path("img/enemy21.png");

    const auto hero_img_path = get_full_data_path("img/hero.png");
    const auto hero_back_img_path = get_full_data_path("img/hero_back.png");
    const auto hero_side_img_path = get_full_data_path("img/hero_side.png");
    const auto hero_side1_img_path = get_full_data_path("img/hero_side1.png");
    const auto sand_path = get_full_data_path("img/sand.png");

    const auto audio_death_path = get_full_data_path("audio/death.wav");
    const auto audio_land_path = get_full_data_path("audio/land.wav");
    const auto audio_push_path = get_full_data_path("audio/push.wav");
    const auto audio_climb_path = get_full_data_path("audio/climb.wav");
    const auto audio_hero_death_path =
        get_full_data_path("audio/hero_death.wav");

    selector_img = al_load_bitmap(selector_img_path.c_str());
    selector_inv_img = al_load_bitmap(selector_inv_img_path.c_str());

    enemy_img0 = al_load_bitmap(enemy_img0_path.c_str());
    enemy_img01 = al_load_bitmap(enemy_img01_path.c_str());
    enemy_img1 = al_load_bitmap(enemy_img1_path.c_str());
    enemy_img11 = al_load_bitmap(enemy_img11_path.c_str());
    enemy_img2 = al_load_bitmap(enemy_img2_path.c_str());
    enemy_img21 = al_load_bitmap(enemy_img21_path.c_str());

    hero_img = al_load_bitmap(hero_img_path.c_str());
    hero_back_img = al_load_bitmap(hero_back_img_path.c_str());
    hero_side_img = al_load_bitmap(hero_side_img_path.c_str());
    hero_side1_img = al_load_bitmap(hero_side1_img_path.c_str());
    sand = al_load_bitmap(sand_path.c_str());

    audio_death = al_load_sample(audio_death_path.c_str());
    audio_land = al_load_sample(audio_land_path.c_str());
    audio_push = al_load_sample(audio_push_path.c_str());
    audio_climb = al_load_sample(audio_climb_path.c_str());
    audio_hero_death = al_load_sample(audio_hero_death_path.c_str());

    restart(0);
}

void Scene::tick(bool key_pressed[ALLEGRO_KEY_MAX],
                 bool key_repeat[ALLEGRO_KEY_MAX])
{
    double now = al_get_time();

    bool left = key_pressed[ALLEGRO_KEY_LEFT] || key_repeat[ALLEGRO_KEY_LEFT];
    bool right =
        key_pressed[ALLEGRO_KEY_RIGHT] || key_repeat[ALLEGRO_KEY_RIGHT];
    bool up = key_pressed[ALLEGRO_KEY_UP] || key_repeat[ALLEGRO_KEY_UP];
    bool down = key_pressed[ALLEGRO_KEY_DOWN] || key_repeat[ALLEGRO_KEY_DOWN];
    bool hole = key_pressed[ALLEGRO_KEY_SPACE];

    bool prev_pyr = key_pressed[ALLEGRO_KEY_H];
    bool next_pyr = key_pressed[ALLEGRO_KEY_J];

    bool restart_level = key_pressed[ALLEGRO_KEY_R];

    if ((left || right || up || down || next_pyr || prev_pyr))
    {
        move_selector(left ? -1 : (right ? 1 : 0), up ? 1 : (down ? -1 : 0));
        if (next_pyr)
            curr_pyramid++;
        if (prev_pyr)
            curr_pyramid--;

        if (curr_pyramid < 0)
            curr_pyramid = 3;

        curr_pyramid = curr_pyramid % 4;
    }

    auto& pyr = pyramids[curr_pyramid];

    selector_active = (now - last_hole_time) > HOLE_RETRY_TIME;

    if (hole && selector_active &&
        pyr.invert_block_at(selector_pos.first, selector_pos.second, 5.0))
    {
        last_hole_time = now;
        play_push_sound = true;
    }

    double dt = std::min(0.05, now - last_update);

    for (int i = 0; i < 4; i++)
    {
        pyramids[i].update(dt);
        update_enemies(dt, pyramids[i], pyramid_enemies[i]);
    }

    bool enemies_dead = true;

    for (int i = 0; i < 4; i++)
    {
        enemies_dead = enemies_dead && pyramid_enemies[i].empty();
    }

    move_enemies_across_edges();

    enemy_sprite_idx = static_cast<int>(now / 0.15) % 2;

    if (dead && !dead_last)
    {
        restart_countdown = 2.9;
        play_hero_death_sound = true;
    }
    else if (dead)
    {
        restart_countdown -= dt;
    }

    if (restart_level || (dead && restart_countdown <= 0.0))
        restart(curr_level);

    if (enemies_dead)
        restart(curr_level + 1);

    if (show_level_countdown > 0.0)
        show_level_countdown -= dt;

    if (dead_enemy)
        al_play_sample(audio_death, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE,
                       nullptr);

    if (play_land_sound)
        al_play_sample(audio_land, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);

    if (play_push_sound)
        al_play_sample(audio_push, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);

    if (play_climb_sound)
        al_play_sample(audio_climb, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE,
                       nullptr);

    if (play_hero_death_sound)
        al_play_sample(audio_hero_death, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE,
                       nullptr);

    draw_level = show_level_countdown >= 0.0;
    dead_last = dead;
    last_update = now;
    dead_enemy = false;
    play_land_sound = false;
    play_push_sound = false;
    play_climb_sound = false;
    play_hero_death_sound = false;
}

void Scene::update_enemies(double dt, Pyramid& pyr, std::vector<Enemy>& enemies)
{
    for (auto it = enemies.begin(); it != enemies.end();)
    {
        BlockType block = pyr.get_block_at(it->pos_col, it->pos_row);

        if (block == SNAKE || block == BLOCK_IN_SNAKE)
        {
            dead_enemy = true;
            it = enemies.erase(it);
            continue;
        }

        if (it->pos_col == hero_pos_col && it->pos_row == hero_pos_row)
            dead = true;

        Enemy& e = (*it);
        block = pyr.get_block_at(e.pos_col, e.pos_row);

        bool start_falling =
            e.pos_row > 0 &&
            (pyr.get_block_at(e.pos_col, e.pos_row - 1) == BLOCK_IN ||
             pyr.get_block_at(e.pos_col, e.pos_row - 1) == BLOCK_IN_SNAKE);
        bool start_climbing = pyr.get_block_at(e.pos_col, e.pos_row) == LADDER;

        double mid_p_col = static_cast<double>(e.pos_col) + 0.5;
        double mid_p_row = static_cast<double>(e.pos_row) + 0.5;

        bool past_mid_x =
            (e.direction_x == -1 && (mid_p_col >= e.pos_col_exact)) ||
            (e.direction_x == 1 && (mid_p_col <= e.pos_col_exact));

        bool past_mid_y =
            (e.state == EnemyState::CLIMBING &&
             (mid_p_row >= e.pos_row_exact)) ||
            (e.state == EnemyState::FALLING && (mid_p_row <= e.pos_row_exact));

        if (e.state == EnemyState::WALKING)
        {
            e.pos_col_exact +=
                static_cast<double>(e.direction_x) * dt * e.speed;

            if (past_mid_x && start_climbing)
            {
                e.state = EnemyState::CLIMBING;
                play_climb_sound = true;
            }
            else if (start_falling && block != LADDER)
            {
                e.state = EnemyState::FALLING;
            }
            else if (e.pos_col != static_cast<int>(e.pos_col_exact))
            {
                e.pos_col = e.pos_col_exact;
                if (e.pos_col < 0 || e.pos_col >= PYR_COLS ||
                    (pyr.get_block_at(e.pos_col, e.pos_row) == EMPTY))
                {
                    // Switch to other pyramid side
                    e.move_next_side = true;

                    // e.pos_col = std::min(PYR_COLS - 1, e.pos_col);
                    // e.pos_col = std::max(0, e.pos_col);
                    //
                    // e.direction_x = -e.direction_x;
                }
            }
        }
        else if (e.state == EnemyState::CLIMBING)
        {
            e.pos_row_exact += dt * e.speed;

            bool climb_done =
                static_cast<int>(std::floor(e.pos_row_exact)) > e.pos_row;

            if (climb_done)
            {
                e.pos_row++;
                e.pos_row_exact = e.pos_row;

                auto block_next = pyr.get_block_at(e.pos_col, e.pos_row);
                e.state = block_next != LADDER ? EnemyState::WALKING
                                               : EnemyState::CLIMBING;
            }
        }
        else if (e.state == EnemyState::FALLING)
        {
            e.pos_row_exact -= dt * e.fall_speed;

            bool fall_done =
                static_cast<int>(std::ceil(e.pos_row_exact)) < e.pos_row;
            if (fall_done)
            {
                e.pos_row--;
                e.pos_row_exact = e.pos_row;

                bool next_fall =
                    (e.pos_row > 0 &&
                     pyr.get_block_at(e.pos_col, e.pos_row - 1) == BLOCK_IN);

                e.state = next_fall ? EnemyState::FALLING : EnemyState::WALKING;

                play_land_sound = play_land_sound || !next_fall;
            }
        }
        it++;
    }
}

void Scene::move_enemies_across_edges()
{
    for (int i = 0; i < 4; i++)
    {
        int prev_i = i == 0 ? 3 : i - 1;
        int next_i = (i + 1) % 4;

        auto& enemies = pyramid_enemies[i];

        for (auto it = enemies.begin(); it != enemies.end();)
        {
            Enemy& e = *it;
            if (e.move_next_side)
            {
                e.move_next_side = false;
                if (e.pos_col > PYR_COLS / 2)
                {
                    e.pos_col = (PYR_COLS - e.pos_col);
                    e.pos_col_exact = e.pos_col;
                }
                else
                {
                    e.pos_col = (PYR_COLS - 2) - e.pos_col;
                    e.pos_col_exact = e.pos_col + 0.5;
                }
                if (e.direction_x == 1)
                {
                    pyramid_enemies[next_i].push_back(e);
                }
                else if (e.direction_x == -1)
                {
                    pyramid_enemies[prev_i].push_back(e);
                }
                else
                {
                    assert(false);
                }

                it = enemies.erase(it);
                continue;
            }
            it++;
        }
    }
}

void Scene::restart(std::size_t level_idx)
{
    dead = false;

    curr_level = level_idx;

    Level level = generate_level(curr_level);

    pyramids = level.pyramids;
    pyramid_enemies = level.pyramid_enemies;

    int i = 0;
    for (auto& enemies : pyramid_enemies)
    {
        for (auto& e : enemies)
        {
            switch (i % 3)
            {
            case 0:
                e.image = enemy_img0;
                e.image1 = enemy_img01;
                break;
            case 1:
                e.image = enemy_img1;
                e.image1 = enemy_img11;
                break;
            case 2:
                e.image = enemy_img2;
                e.image1 = enemy_img21;
                break;
            }
            i++;
        }
    }

    show_level_countdown = 2.0;
    draw_level = true;
    curr_pyramid = 0;
    last_hole_time = 0;
}

void Scene::draw_text(const char* str)
{
    int tw = al_get_text_width(font, str) + 10;
    int th = al_get_font_line_height(font) + 4;
    int x0 = L_WIDTH / 2 - tw / 2;
    int y0 = L_HEIGHT / 2 - 4;
    int x1 = L_WIDTH / 2 + tw / 2;
    int y1 = L_HEIGHT / 2 + th;
    al_draw_filled_rectangle(x0, y0, x1, y1, al_map_rgb(0, 0, 0));
    al_draw_rectangle(x0, y0, x1, y1, al_map_rgb(125, 125, 125), 1);

    al_draw_text(font, al_map_rgb(255, 255, 255), L_WIDTH / 2, L_HEIGHT / 2,
                 ALLEGRO_ALIGN_CENTRE, str);
}

void Scene::generate_stars()
{
    stars.clear();

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distribution(0,
                                                                          1000);
    std::uniform_int_distribution<std::mt19937::result_type> distribution6(0,
                                                                           6);

    for (int i = 0; i < 1000; ++i)
    {
        Star star;
        star.x = distribution(rng);
        star.y = distribution(rng);
        if (distribution6(rng) < 4)
        {
            star.color = al_map_rgb(252, 228, 160);
        }
        else
        {
            star.color = al_map_rgb(136, 112, 0);
        }
        stars.push_back(star);
    }
}

void Scene::draw_stars(int width, int height)
{
    if (stars.empty())
    {
        generate_stars();
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distribution(0,
                                                                          500);

    for (Star& star : stars)
    {
        star.sparkle_countdown = std::max(0, star.sparkle_countdown - 1);
        const bool sparkle = distribution(rng) == 0;
        if (sparkle)
        {
            star.sparkle_countdown = 30;
        }
    }

    for (int i = -1; i < (width + 1000) / 1000; i++)
    {
        for (int j = -1; j < (height + 1000) / 1000; j++)
        {
            for (const Star& star : stars)
            {
                int x = 1000 * i;
                int y = 1000 * j;
                ALLEGRO_COLOR color = star.sparkle_countdown > 0
                                          ? al_map_rgb(255, 255, 255)
                                          : star.color;
                al_draw_filled_rectangle(x + star.x, y + star.y, x + star.x + 1,
                                         y + star.y + 1, color);
            }
        }
    }
}

void Scene::draw_sand(int width, int height)
{
    const ALLEGRO_COLOR sand_color = al_map_rgb(136, 112, 0);

    constexpr int center_x = L_WIDTH / 2;
    const int min_x = (L_WIDTH - width) / 2;
    const int max_x = (L_WIDTH + width) / 2;
    const int max_y = (L_HEIGHT + height) / 2;

    al_draw_filled_rectangle(min_x, L_HEIGHT, max_x, L_HEIGHT + max_y,
                             sand_color);

    const int image_width = al_get_bitmap_width(sand);
    const int image_height = al_get_bitmap_height(sand);
    const int image_y = L_HEIGHT;

    al_draw_bitmap(sand, 0, image_y, 0);

    bool done = false;
    int i = 0;
    while (!done)
    {
        ++i;
        done = true;
        int end_x = (1 - i) * image_width;
        if (end_x >= min_x)
        {
            done = false;
            al_draw_bitmap(sand, -i * image_width, image_y, 0);
        }

        int start_x = i * image_width;
        if (start_x <= max_x)
        {
            al_draw_bitmap(sand, i * image_width, image_y, 0);
            done = false;
        }
    }
}

void Scene::draw(int width, int height)
{
    draw_stars(width, height);

    auto& pyr = pyramids[curr_pyramid];
    pyr.draw();

    int h_x = Pyramid::get_block_x(hero_pos_col);
    int h_y = Pyramid::get_block_y(hero_pos_row);

    if (curr_pyramid == 0)
        al_draw_bitmap(hero_img, h_x, h_y, 0);
    else if (curr_pyramid == 1)
        al_draw_bitmap(hero_side1_img, h_x, h_y, 0);
    else if (curr_pyramid == 3)
        al_draw_bitmap(hero_side_img, h_x, h_y, 0);
    else
        al_draw_bitmap(hero_back_img, h_x, h_y, 0);

    for (auto& e : pyramid_enemies[curr_pyramid])
    {
        auto image = enemy_sprite_idx == 0 ? e.image : e.image1;
        al_draw_bitmap(image, Pyramid::get_block_x(e.pos_col_exact - 0.5),
                       Pyramid::get_block_y(e.pos_row_exact), 0);
    }

    auto sel_img = selector_active ? selector_img : selector_inv_img;
    al_draw_bitmap(sel_img, Pyramid::get_block_x(selector_pos.first) - 2,
                   Pyramid::get_block_y(selector_pos.second) - 2, 0);

    if (draw_level)
    {
        char str[200];
        snprintf(str, 200, "Level %d", curr_level + 1);
        draw_text(str);
    }

    if (dead)
    {
        char str[200];
        int secs_left = static_cast<int>(restart_countdown);
        snprintf(str, 200, "You Died, Restarting in %d", secs_left + 1);
        draw_text(str);
    }

    draw_sand(width, height);

#if 0
    // Origo
    al_draw_rectangle(-1000, 0, 1000, 0, al_map_rgb(255, 0, 0), 1);
    al_draw_rectangle(0, -1000, 0, 1000, al_map_rgb(0, 255, 0), 1);

    // Viewport
    al_draw_rectangle(0, 0, L_WIDTH, L_HEIGHT, al_map_rgb(0, 0, 255), 1);
#endif
}

void Scene::move_selector(int col_dt, int row_dt)
{
    selector_pos.first += col_dt;
    selector_pos.second += row_dt;

    if (selector_pos.first >= PYR_COLS)
        selector_pos.first = PYR_COLS - 1;

    if (selector_pos.second >= PYR_ROWS)
        selector_pos.second = PYR_ROWS - 1;

    if (selector_pos.first < 0)
        selector_pos.first = 0;

    if (selector_pos.second < 0)
        selector_pos.second = 0;
}
