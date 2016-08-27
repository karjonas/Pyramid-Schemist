#include "scene.h"
#include "pyramid.h"
#include "layout.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

Scene::Scene() {
  selector_img = al_load_bitmap("img/selector.png");


  enemy_img0 = al_load_bitmap("img/enemy.png");
  enemy_img1 = al_load_bitmap("img/enemy1.png");
  enemy_img2 = al_load_bitmap("img/enemy2.png");

{
  Enemy e;
  e.pos = {5,3};
  e.pos_exact = {5,3};
  e.image = enemy_img0;
  enemies.push_back(e);
}
{
  Enemy e;
  e.pos = {6,2};
  e.pos_exact = {6,2};
  e.image = enemy_img1;
  enemies.push_back(e);
}
{
  Enemy e;
  e.pos = {3,0};
  e.pos_exact = {3,0};
  e.image = enemy_img2;
  enemies.push_back(e);
}

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
    if (e.state == EnemyState::WALKING) {
        e.pos_exact.first += static_cast<double>(e.direction)*dt*e.speed;
        e.pos.first = e.pos_exact.first;
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
      al_draw_bitmap(e.image, offsetw + w*e.pos_exact.first, L_HEIGHT - (offseth + h*e.pos_exact.second), 0);
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
