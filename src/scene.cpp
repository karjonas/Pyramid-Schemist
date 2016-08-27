#include "scene.h"
#include "pyramid.h"
#include "layout.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

Scene::Scene() {
  selector_img = al_load_bitmap("img/selector.png");
}

void Scene::tick(ALLEGRO_KEYBOARD_STATE* kbd_state) {
  bool left = al_key_down(kbd_state,ALLEGRO_KEY_LEFT) || al_key_down(kbd_state, ALLEGRO_KEY_A);
  bool right = al_key_down(kbd_state,ALLEGRO_KEY_RIGHT) || al_key_down(kbd_state, ALLEGRO_KEY_D);
  bool up = al_key_down(kbd_state,ALLEGRO_KEY_UP) || al_key_down(kbd_state, ALLEGRO_KEY_W);
  bool down = al_key_down(kbd_state,ALLEGRO_KEY_DOWN) || al_key_down(kbd_state, ALLEGRO_KEY_S);

  double now = al_get_time();

  if ((now - last_press_time >= 0.1) && (left || right || up || down)) {
    move_selector(left ? -1 : (right ? 1 : 0), up ? -1 : (down ? 1 : 0) );
    last_press_time = now;
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
  al_draw_bitmap(selector_img, offsetw + w*selector_pos.first, offseth + h*selector_pos.second, 0);

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
