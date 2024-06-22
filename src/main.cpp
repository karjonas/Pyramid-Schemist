#include "layout.h"
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

#include "paths.h"

bool done;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;
ALLEGRO_DISPLAY *display;
ALLEGRO_FONT *font;

void abort_game(const char *message)
{
    printf("%s \n", message);
    exit(1);
}

void init(void)
{
    if (!al_init())
        abort_game("Failed to initialize allegro");

    if (!al_install_keyboard())
        abort_game("Failed to install keyboard");
    if (!al_init_primitives_addon())
        abort_game("Failed to install primitives");
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon();  // initialize the ttf (True Type Font) addon

    if (!al_install_audio())
    {
        abort_game("failed to initialize audio!\n");
    }

    if (!al_init_acodec_addon())
    {
        abort_game("failed to initialize audio codecs!\n");
    }

    if (!al_reserve_samples(10))
    {
        abort_game("failed to reserve samples!\n");
    }

    if (!al_init_image_addon())
    {
        abort_game("failed to init image addon!\n");
    }

    //  timer = al_create_timer(1.0 / 60);
    //  if (!timer)
    //    abort_game("Failed to create timer");

    ALLEGRO_DISPLAY_MODE disp_data;
    al_get_display_mode(0, &disp_data);

    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    // display = al_create_display(disp_data.width, disp_data.height);
    display = al_create_display(640, 400);
    if (!display)
        abort_game("Failed to create display");

    event_queue = al_create_event_queue();
    if (!event_queue)
        abort_game("Failed to create event queue");

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    //  al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    const auto font_path = get_full_data_path("fonts/Atari_Full.ttf");
    font = al_load_ttf_font(font_path.c_str(), 8, 0);

    ALLEGRO_TRANSFORM trans;
    al_identity_transform(&trans);
    al_scale_transform(&trans, 2, 2);

    al_use_transform(&trans);

    done = false;
}

void shutdown(void)
{
    if (timer)
        al_destroy_timer(timer);

    if (display)
        al_destroy_display(display);

    if (event_queue)
        al_destroy_event_queue(event_queue);

    al_shutdown_primitives_addon();
}

void game_loop(void)
{
    Scene scene;
    scene.font = font;
    ALLEGRO_KEYBOARD_STATE kbd_state;

    bool key_pressed[ALLEGRO_KEY_MAX] = {0};
    bool key_down[ALLEGRO_KEY_MAX] = {0};

    double key_time[ALLEGRO_KEY_MAX] = {0};

    bool key_repeat[ALLEGRO_KEY_MAX] = {0};
    bool key_repeat_first[ALLEGRO_KEY_MAX] = {0};

    constexpr double KEY_REP_TIME = 0.1;
    constexpr double KEY_REP_TIME_FIRST = 0.35;

    while (!done)
    {
        ALLEGRO_EVENT ev;
        double now = al_get_time();

        while (al_get_next_event(event_queue, &ev))
        {
            if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                key_pressed[ev.keyboard.keycode] = true;
                key_down[ev.keyboard.keycode] = true;
                key_time[ev.keyboard.keycode] = now;
                key_repeat_first[ev.keyboard.keycode] = true;
            }
            else if (ev.type == ALLEGRO_EVENT_KEY_UP)
            {
                key_down[ev.keyboard.keycode] = false;
                key_repeat_first[ev.keyboard.keycode] = false;
                key_repeat[ev.keyboard.keycode] = false;
            }
            else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                done = true;
            }
            else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
            {
                int width = ev.display.width;
                int height = ev.display.height;
                float scale_x = float(width) / L_WIDTH;
                float scale_y = float(height) / L_HEIGHT;
                float scale = std::max(int(std::min(scale_x, scale_y)), 1);
                float scaled_width = L_WIDTH * scale;
                float scaled_height = L_HEIGHT * scale;
                int offset_x = (width - scaled_width) * 0.5f;
                int offset_y = (height - scaled_height) * 0.5f;

                ALLEGRO_TRANSFORM trans;
                al_identity_transform(&trans);
                al_scale_transform(&trans, scale, scale);
                al_translate_transform(&trans, offset_x, offset_y);
                al_use_transform(&trans);
                al_acknowledge_resize(display);
            }
        }

        if (key_down[ALLEGRO_KEY_ESCAPE])
            done = true;

        for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
        {
            double rep_time =
                key_repeat_first[i] ? KEY_REP_TIME_FIRST : KEY_REP_TIME;
            if (key_down[i] && (now - key_time[i] >= rep_time))
            {
                key_time[i] = now;
                key_repeat[i] = true;
                key_repeat_first[i] = false;
            }
        }

        scene.tick(key_pressed, key_repeat);
        al_clear_to_color(al_map_rgb(0, 0, 0));
        scene.draw(al_get_display_width(display),
                   al_get_display_height(display));
        al_flip_display();

        for (auto &pressed : key_pressed)
            pressed = false;

        for (auto &rep : key_repeat)
            rep = false;
    }
}

int main(int argc, char *argv[])
{
    init();
    game_loop();
    shutdown();
}
