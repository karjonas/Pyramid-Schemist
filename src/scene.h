#pragma once

#include "pyramid.h"
#include "enemy.h"

#include <utility>
#include <vector>
#include <allegro5/color.h>

struct ALLEGRO_KEYBOARD_STATE;
struct ALLEGRO_FONT;
struct ALLEGRO_SAMPLE;

struct Star
{
    int x = 0;
    int y = 0;
    int sparkle_countdown = 0;
    ALLEGRO_COLOR color;
};

class Scene
{
public:
    Scene();

    void tick(bool key_pressed[], bool key_repeat[]);
    void draw(int width, int height);

    void move_selector(int col_dt, int row_dt);
    void update_enemies(double dt, Pyramid &pyr, std::vector<Enemy> &enemies);

    void move_enemies_across_edges();
    void restart(std::size_t level_idx);

    void draw_sand(int width, int height);
    void draw_text(const char *str);
    void draw_stars(int width, int height);
    void generate_stars();

    std::array<Pyramid, 4> pyramids;
    std::vector<Star> stars;

    std::pair<int, int> selector_pos = {4, 3};
    ALLEGRO_BITMAP *selector_img;
    ALLEGRO_BITMAP *selector_inv_img;

    double last_update = 0;

    std::array<std::vector<Enemy>, 4> pyramid_enemies;

    ALLEGRO_BITMAP *enemy_img0;
    ALLEGRO_BITMAP *enemy_img1;
    ALLEGRO_BITMAP *enemy_img2;

    ALLEGRO_BITMAP *enemy_img01;
    ALLEGRO_BITMAP *enemy_img11;
    ALLEGRO_BITMAP *enemy_img21;

    ALLEGRO_BITMAP *hero_img;
    ALLEGRO_BITMAP *hero_back_img;
    ALLEGRO_BITMAP *hero_side_img;
    ALLEGRO_BITMAP *hero_side1_img;

    ALLEGRO_BITMAP *sand;

    ALLEGRO_SAMPLE *audio_death;
    ALLEGRO_SAMPLE *audio_land;
    ALLEGRO_SAMPLE *audio_push;
    ALLEGRO_SAMPLE *audio_climb;
    ALLEGRO_SAMPLE *audio_hero_death;

    double last_hole_time = 0.0;
    std::size_t curr_pyramid = 0;

    ALLEGRO_FONT *font;
    bool dead = false;
    bool dead_last = false;
    std::size_t hero_pos_row = 7;
    std::size_t hero_pos_col = 6;
    std::size_t curr_level = 0;

    double restart_countdown = 0.0;
    double show_level_countdown = 0.0;
    bool draw_level = false;

    bool dead_enemy = false;
    bool play_land_sound = false;
    bool play_push_sound = false;
    bool play_climb_sound = false;
    bool play_hero_death_sound = false;

    bool selector_active = true;
    int enemy_sprite_idx = 0;

    double elapsed_time = 0;
};
