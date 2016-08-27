#include "scene.h"
#include "pyramid.h"
#include "layout.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include <cmath>

#define HOLE_RETRY_TIME 1.0


Scene::Scene() {
  selector_img = al_load_bitmap("img/selector.png");


  enemy_img0 = al_load_bitmap("img/enemy.png");
  enemy_img1 = al_load_bitmap("img/enemy1.png");
  enemy_img2 = al_load_bitmap("img/enemy2.png");

{
  Enemy e;
  e.pos_row = 1;
  e.pos_row_exact = 1;

  e.pos_col = 10;
  e.pos_col_exact = 10.5;

  e.image = enemy_img0;
  enemies.push_back(e);
}

  {
    Enemy e;
    e.pos_row = 0;
    e.pos_row_exact = 0;

    e.pos_col = 0;
    e.pos_col_exact = 0.5;

    e.image = enemy_img0;
    e.direction_x = 1;
    enemies.push_back(e);
  }

//{
//  Enemy e;
//  e.pos = {6,2};
//  e.pos_exact = {6,2};
//  e.image = enemy_img1;
//  enemies.push_back(e);
//}
//{
//  Enemy e;
//  e.pos = {3,0};
//  e.pos_exact = {3,0};
//  e.image = enemy_img2;
//  enemies.push_back(e);
//}

}

void Scene::tick(ALLEGRO_KEYBOARD_STATE* kbd_state) {
  double now = al_get_time();

  bool left = al_key_down(kbd_state,ALLEGRO_KEY_LEFT) || al_key_down(kbd_state, ALLEGRO_KEY_A);
  bool right = al_key_down(kbd_state,ALLEGRO_KEY_RIGHT) || al_key_down(kbd_state, ALLEGRO_KEY_D);
  bool up = al_key_down(kbd_state,ALLEGRO_KEY_UP) || al_key_down(kbd_state, ALLEGRO_KEY_W);
  bool down = al_key_down(kbd_state,ALLEGRO_KEY_DOWN) || al_key_down(kbd_state, ALLEGRO_KEY_S);
  bool hole = al_key_down(kbd_state,ALLEGRO_KEY_SPACE) || al_key_down(kbd_state, ALLEGRO_KEY_H);

  if ((now - last_press_time >= 0.1) && (left || right || up || down)) {
    move_selector(left ? -1 : (right ? 1 : 0), up ? 1 : (down ? -1 : 0) );
    last_press_time = now;
  }

  if (hole && ((now - last_hole_time) > HOLE_RETRY_TIME)
      && pyr.invert_block_at(selector_pos.first, selector_pos.second, 5.0)) {
     last_hole_time = now;
  }

  double dt = std::min(0.05, now - last_update);

  pyr.update(dt);
  update_enemies(dt);


  last_update = now;
}

void Scene::update_enemies(double dt) {

  for (Enemy& e : enemies) {
    BlockType block = pyr.get_block_at(e.pos_col, e.pos_row);

    bool start_falling  = e.pos_row > 0 && pyr.get_block_at(e.pos_col, e.pos_row - 1) == BLOCK_IN;
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
            e.pos_col = std::min(PYR_COLS - 1, e.pos_col);
            e.pos_col = std::max(0, e.pos_col);

            e.direction_x = -e.direction_x;
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
      e.pos_row_exact -= dt*e.speed;

      bool fall_done = static_cast<int>(std::ceil(e.pos_row_exact)) < e.pos_row;
      if (fall_done) {
        e.pos_row--;
        e.pos_row_exact = e.pos_row;

        bool next_fall = (e.pos_row > 0 && pyr.get_block_at(e.pos_col, e.pos_row -1) == BLOCK_IN);

        e.state = next_fall ? EnemyState::FALLING : EnemyState::WALKING;
      }
    }
  }

}

void Scene::draw() {
  constexpr int w = 21;
  constexpr int h = 16;

  constexpr int totw = w * PYR_COLS;
  constexpr int toth = h * PYR_ROWS;

  constexpr int offsetw = (L_WIDTH - totw) / 2;
  constexpr int offseth = (L_HEIGHT - toth) / 2;

  pyr.draw();

  for (auto& e : enemies) {
      al_draw_bitmap(e.image, offsetw + w*e.pos_col_exact -w/2, L_HEIGHT - (offseth + h*e.pos_row_exact), 0);
      int x1 = offsetw + e.pos_col*w;
      int y1 = L_HEIGHT - (offseth + h*e.pos_row);
      int x2 = x1 + w;
      int y2 = y1 + h;

//      al_draw_rectangle(x1+0.5,y1+0.5,x2+0.5,y2+0.5,al_map_rgb(255,0,0), 1.0);
    }

  al_draw_bitmap(selector_img, offsetw + w*selector_pos.first, L_HEIGHT - (offseth + h*selector_pos.second), 0);

}

void Scene::move_selector(int col_dt, int row_dt) {
  selector_pos.first += col_dt;
  selector_pos.second += row_dt;

 if (selector_pos.first >= PYR_COLS)
   selector_pos.first = 0;

 if (selector_pos.second >= PYR_ROWS)
   selector_pos.second = 0;

 if (selector_pos.first < 0)
   selector_pos.first = (PYR_COLS - 1);

 if (selector_pos.second < 0)
   selector_pos.second = (PYR_ROWS - 1);
}
