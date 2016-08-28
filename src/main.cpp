#include "scene.h"

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>

bool done;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;
ALLEGRO_DISPLAY *display;
ALLEGRO_FONT *font;

void abort_game(const char *message) {
  printf("%s \n", message);
  exit(1);
}

void init(void) {
  if (!al_init())
    abort_game("Failed to initialize allegro");

  if (!al_install_keyboard())
    abort_game("Failed to install keyboard");
  if (!al_init_primitives_addon())
    abort_game("Failed to install primitives");
  al_init_font_addon(); // initialize the font addon
  al_init_ttf_addon();  // initialize the ttf (True Type Font) addon

  if (!al_install_audio()) {
    abort_game("failed to initialize audio!\n");
  }

  if (!al_init_acodec_addon()) {
    abort_game("failed to initialize audio codecs!\n");
  }

  if (!al_reserve_samples(1)) {
    abort_game("failed to reserve samples!\n");
  }

  if (!al_init_image_addon()) {
    abort_game("failed to init image addon!\n");
  }

//  timer = al_create_timer(1.0 / 60);
//  if (!timer)
//    abort_game("Failed to create timer");

  ALLEGRO_DISPLAY_MODE disp_data;
  al_get_display_mode(0, &disp_data);

  al_set_new_display_flags(ALLEGRO_WINDOWED);
  // display = al_create_display(disp_data.width, disp_data.height);
  display = al_create_display(320, 200);
  if (!display)
    abort_game("Failed to create display");

  event_queue = al_create_event_queue();
  if (!event_queue)
    abort_game("Failed to create event queue");

  al_register_event_source(event_queue, al_get_keyboard_event_source());
//  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_display_event_source(display));

  al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

  font = al_load_ttf_font("src/Atari_Full.ttf", 8, 0);

  done = false;
}

void shutdown(void) {
  if (timer)
    al_destroy_timer(timer);

  if (display)
    al_destroy_display(display);

  if (event_queue)
    al_destroy_event_queue(event_queue);

  al_shutdown_primitives_addon();
}

void game_loop(void) {
  Scene scene;
  scene.font = font;
  ALLEGRO_KEYBOARD_STATE kbd_state;

  bool key_pressed[ALLEGRO_KEY_MAX] = {0};
  bool key_down[ALLEGRO_KEY_MAX] = {0};

  while (!done) {
    ALLEGRO_EVENT ev;
    while (al_get_next_event(event_queue, &ev)) {
      if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
        key_pressed[ev.keyboard.keycode] = true;
        key_down[ev.keyboard.keycode] = true;
      } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
        key_down[ev.keyboard.keycode] = false;
      } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        done = false;
      }
    }

    if (key_down[ALLEGRO_KEY_ESCAPE])
      done = false;

    scene.tick(key_pressed);
    al_clear_to_color(al_map_rgb(0,0,0));
    scene.draw();
    al_flip_display();

    for (auto &pressed : key_pressed)
      pressed = false;
  }
}

int main(int argc, char *argv[]) {
  init();
  game_loop();
  shutdown();
}
