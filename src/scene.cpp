#include "scene.h"
#include "pyramid.h"
#include "layout.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

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

    e.pos_col = 12;
    e.pos_col_exact = 12.5;

    e.image = enemy_img0;
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


  if ((now - last_press_time >= 0.1) && (left || right || up || down)) {
    move_selector(left ? -1 : (right ? 1 : 0), up ? 1 : (down ? -1 : 0) );
    last_press_time = now;
  }

  double dt = std::min(0.05, now - last_update);

  update_enemies(dt);


  last_update = now;
}

void Scene::update_enemies(double dt) {

  for (Enemy& e : enemies) {
    BlockType block = pyr.get_block_at(e.pos_col, e.pos_row);

    if (block == LADDER && e.state != EnemyState::CLIMBING) {
      double mid_p = static_cast<double>(e.pos_col) + 0.5;

      if (  (e.direction_x == -1 && (mid_p >= e.pos_col_exact))
          ||(e.direction_x ==  1 && (mid_p <= e.pos_col_exact))) {
        e.pos_col_exact = mid_p; // #HACK For snapping to middle
        e.state = EnemyState::CLIMBING;
      }
    } else if (block == BLOCK) {
      e.pos_row_exact = e.pos_row;
      e.state = EnemyState::WALKING;
    }

    if (e.state == EnemyState::WALKING) {
        e.pos_col_exact += static_cast<double>(e.direction_x)*dt*e.speed;
        e.pos_col = e.pos_col_exact;
    } else if (e.state == EnemyState::CLIMBING) {
        e.pos_row_exact += dt*e.speed;
        e.pos_row = e.pos_row_exact;
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

      al_draw_rectangle(x1+0.5,y1+0.5,x2+0.5,y2+0.5,al_map_rgb(255,0,0), 1.0);
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
