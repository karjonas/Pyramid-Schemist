#include "scene.h"
#include "pyramid.h"
#include "layout.h"
#include "level.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include <cmath>

#define HOLE_RETRY_TIME 1.0


Scene::Scene() {
  selector_img = al_load_bitmap("img/selector.png");
  enemy_img0 = al_load_bitmap("img/enemy.png");
  enemy_img1 = al_load_bitmap("img/enemy1.png");
  enemy_img2 = al_load_bitmap("img/enemy2.png");
  hero_img = al_load_bitmap("img/hero.png");
  hero_back_img = al_load_bitmap("img/hero_back.png");
  hero_side_img = al_load_bitmap("img/hero_side.png");
  hero_side1_img = al_load_bitmap("img/hero_side1.png");

  restart(0);
}

void Scene::tick(bool key_pressed[ALLEGRO_KEY_MAX]) {
  double now = al_get_time();

  bool left  = key_pressed[ALLEGRO_KEY_LEFT ] || key_pressed[ALLEGRO_KEY_A];
  bool right = key_pressed[ALLEGRO_KEY_RIGHT] || key_pressed[ALLEGRO_KEY_D];
  bool up    = key_pressed[ALLEGRO_KEY_UP   ] || key_pressed[ALLEGRO_KEY_W];
  bool down  = key_pressed[ALLEGRO_KEY_DOWN ] || key_pressed[ALLEGRO_KEY_S];
  bool hole  = key_pressed[ALLEGRO_KEY_SPACE];

  bool prev_pyr = key_pressed[ALLEGRO_KEY_H];
  bool next_pyr = key_pressed[ALLEGRO_KEY_J];

  if ((now - last_press_time >= 0.1) && (left || right || up || down || next_pyr || prev_pyr)) {
    move_selector(left ? -1 : (right ? 1 : 0), up ? 1 : (down ? -1 : 0) );
    if (next_pyr)
      curr_pyramid++;
    if (prev_pyr)
      curr_pyramid--;

    if (curr_pyramid < 0)
      curr_pyramid = 3;

    curr_pyramid = curr_pyramid % 4;

    last_press_time = now;
  }

  auto& pyr = pyramids[curr_pyramid];

  if (hole && ((now - last_hole_time) > HOLE_RETRY_TIME)
      && pyr.invert_block_at(selector_pos.first, selector_pos.second, 5.0)) {
     last_hole_time = now;
  }

  double dt = std::min(0.05, now - last_update);

  for (int i = 0; i < 4; i++) {
    pyramids[i].update(dt);
    update_enemies(dt, pyramids[i], pyramid_enemies[i]);
  }

  bool enemies_dead = true;

  for (int i = 0; i < 4; i++) {
    enemies_dead = enemies_dead && pyramid_enemies[i].empty();
  }

  move_enemies_across_edges();

  if (dead && !dead_last) {
    restart_countdown = 2.9;
  } else if (dead) {
    restart_countdown -= dt;
  }

  if (dead && restart_countdown <= 0.0)
    restart(curr_level);

  if (enemies_dead)
    restart(curr_level + 1);

  if (show_level_countdown > 0.0)
    show_level_countdown -= dt;

  draw_level = show_level_countdown >= 0.0;
  dead_last = dead;
  last_update = now;
}

void Scene::update_enemies(double dt, Pyramid& pyr, std::vector<Enemy>& enemies) {

  for (auto it = enemies.begin(); it != enemies.end();) {
    BlockType block = pyr.get_block_at(it->pos_col, it->pos_row);

    if (block == SNAKE || block == BLOCK_IN_SNAKE) {
      it = enemies.erase(it);
      continue;
    }

    if (it->pos_col == hero_pos_col && it->pos_row == hero_pos_row)
        dead = true;

    Enemy& e = (*it);
    block = pyr.get_block_at(e.pos_col, e.pos_row);

    bool start_falling  = e.pos_row > 0 &&
        ( pyr.get_block_at(e.pos_col, e.pos_row - 1) == BLOCK_IN
         || pyr.get_block_at(e.pos_col, e.pos_row - 1) == BLOCK_IN_SNAKE);
    bool start_climbing = pyr.get_block_at(e.pos_col, e.pos_row) == LADDER;

    double mid_p_col = static_cast<double>(e.pos_col) + 0.5;
    double mid_p_row = static_cast<double>(e.pos_row) + 0.5;

    bool past_mid_x = (e.direction_x == -1 && (mid_p_col >= e.pos_col_exact))
        ||(e.direction_x ==  1 && (mid_p_col <= e.pos_col_exact));

    bool past_mid_y = (e.state == EnemyState::CLIMBING && (mid_p_row >= e.pos_row_exact))
        || (e.state == EnemyState::FALLING && (mid_p_row <= e.pos_row_exact));

    if (e.state == EnemyState::WALKING) {
      e.pos_col_exact += static_cast<double>(e.direction_x)*dt*e.speed;

      if (past_mid_x && start_climbing) {
          e.state = EnemyState::CLIMBING;
      } else if (start_falling && block != LADDER) {
          e.state = EnemyState::FALLING;
      } else if (e.pos_col != static_cast<int>(e.pos_col_exact)) {
        e.pos_col = e.pos_col_exact;
        if (e.pos_col < 0 || e.pos_col >= PYR_COLS || (pyr.get_block_at(e.pos_col, e.pos_row) == EMPTY)) {

            // Switch to other pyramid side
            e.move_next_side = true;

            //e.pos_col = std::min(PYR_COLS - 1, e.pos_col);
            //e.pos_col = std::max(0, e.pos_col);
            //
            //e.direction_x = -e.direction_x;
        }
      }
    } else if (e.state == EnemyState::CLIMBING) {
      e.pos_row_exact += dt*e.speed;

      bool climb_done = static_cast<int>(std::floor(e.pos_row_exact)) > e.pos_row;

      if (climb_done) {
          e.pos_row++;
          e.pos_row_exact = e.pos_row;

          auto block_next = pyr.get_block_at(e.pos_col, e.pos_row);
          e.state = block_next != LADDER ? EnemyState::WALKING : EnemyState::CLIMBING;
      }
    } else if (e.state == EnemyState::FALLING) {
      e.pos_row_exact -= dt*e.fall_speed;

      bool fall_done = static_cast<int>(std::ceil(e.pos_row_exact)) < e.pos_row;
      if (fall_done) {
        e.pos_row--;
        e.pos_row_exact = e.pos_row;

        bool next_fall = (e.pos_row > 0 && pyr.get_block_at(e.pos_col, e.pos_row -1) == BLOCK_IN);

        e.state = next_fall ? EnemyState::FALLING : EnemyState::WALKING;
      }
    }
    it++;
  }

}

void Scene::move_enemies_across_edges() {
  for (int i = 0; i < 4; i++) {
    int prev_i = i == 0 ? 3 : i - 1;
    int next_i = (i + 1) % 4;

    auto& enemies = pyramid_enemies[i];

    for (auto it = enemies.begin(); it != enemies.end();) {
      Enemy& e = *it;
      if (e.move_next_side) {
        e.move_next_side = false;
        if (e.pos_col > PYR_COLS/2) {
          e.pos_col = (PYR_COLS - e.pos_col);
          e.pos_col_exact = e.pos_col;
        } else {
          e.pos_col = (PYR_COLS -2) - e.pos_col;
          e.pos_col_exact = e.pos_col + 0.5;
        }
        if (e.direction_x == 1) {
          pyramid_enemies[next_i].push_back(e);
        } else if (e.direction_x == -1) {
          pyramid_enemies[prev_i].push_back(e);
        } else {
          assert(false);
        }

        it = enemies.erase(it);
        continue;
      }
      it++;
    }
  }
}

void Scene::restart(size_t level_idx) {
  dead = false;

  curr_level = level_idx;

  Level level = generate_level(curr_level);

  pyramids = level.pyramids;
  pyramid_enemies = level.pyramid_enemies;

  int i = 0;
  for (auto& enemies : pyramid_enemies) {
    for (auto& e : enemies) {
      switch (i) {
        case 0: e.image = enemy_img0; break;
        case 1: e.image = enemy_img1; break;
        default: e.image = enemy_img2; break;
      }
      i++;
    }
  }

  show_level_countdown = 2.0;
  draw_level = true;
  curr_pyramid = 0;
}

void Scene::draw_text(const char* str)
{
  int tw = al_get_text_width(font, str) + 10;
  int th = al_get_font_line_height(font) + 4;
  int x0 = L_WIDTH/2 -tw/2;
  int y0 = L_HEIGHT/2 -4;
  int x1 = L_WIDTH/2 + tw/2;
  int y1 = L_HEIGHT/2 + th;
  al_draw_filled_rectangle(x0,y0,x1,y1, al_map_rgb(0,0,0));
  al_draw_rectangle(x0,y0,x1,y1, al_map_rgb(125,125,125), 1);

  al_draw_text(font, al_map_rgb(255,255,255), L_WIDTH/2, L_HEIGHT/2, ALLEGRO_ALIGN_CENTRE, str);
}

void Scene::draw() {
  constexpr int w = 21;
  constexpr int h = 16;

  constexpr int totw = w * PYR_COLS;
  constexpr int toth = h * PYR_ROWS;

  constexpr int offsetw = (L_WIDTH - totw) / 2;
  constexpr int offseth = (L_HEIGHT - toth) / 2;

  auto& pyr = pyramids[curr_pyramid];
  pyr.draw();

  int h_x = offsetw + w*hero_pos_col;
  int h_y = L_HEIGHT - (offseth + h*hero_pos_row);
  if (curr_pyramid == 0)
    al_draw_bitmap(hero_img, h_x, h_y, 0);
  else if (curr_pyramid == 1)
    al_draw_bitmap(hero_side1_img, h_x, h_y, 0);
  else if (curr_pyramid == 3)
    al_draw_bitmap(hero_side_img, h_x, h_y, 0);
  else
    al_draw_bitmap(hero_back_img, h_x, h_y, 0);



  for (auto& e : pyramid_enemies[curr_pyramid]) {
      al_draw_bitmap(e.image, offsetw + w*e.pos_col_exact -w/2, L_HEIGHT - (offseth + h*e.pos_row_exact), 0);
      int x1 = offsetw + e.pos_col*w;
      int y1 = L_HEIGHT - (offseth + h*e.pos_row);
      int x2 = x1 + w;
      int y2 = y1 + h;

//      al_draw_rectangle(x1+0.5,y1+0.5,x2+0.5,y2+0.5,al_map_rgb(255,0,0), 1.0);

    }


  al_draw_bitmap(selector_img, offsetw + w*selector_pos.first, L_HEIGHT - (offseth + h*selector_pos.second), 0);


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

}

void Scene::move_selector(int col_dt, int row_dt) {
  selector_pos.first += col_dt;
  selector_pos.second += row_dt;

 if (selector_pos.first >= PYR_COLS)
   selector_pos.first = PYR_COLS - 1;

 if (selector_pos.second >= PYR_ROWS)
   selector_pos.second =  PYR_ROWS -1;

 if (selector_pos.first < 0)
   selector_pos.first = 0;

 if (selector_pos.second < 0)
   selector_pos.second = 0;
}
