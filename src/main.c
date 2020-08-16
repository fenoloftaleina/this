#include "stdio.h"
#include "string.h"

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_time.h"
#define HANDMADE_MATH_IMPLEMENTATION
#include "HandmadeMath.h"

#include "main.glsl.h"

#include "buffer_object.h"
#include "rect.h"
#include "map.h"
#include "input.h"
#include "player.h"
#include "logic.h"
#include "editor.h"


static input_data in = {IN_NONE, IN_NONE};
static bool in_editor = false;

static player_data player;
static map_data map;
static editor_data editor;
static logic_data logic = (logic_data){
  .default_steps_till_eval = 3
};

static sg_pass_action pass_action;

static char cur_map[255];


void run_map(const char* map_name)
{
  strcpy(cur_map, map_name);
  load_map(&map, cur_map);
  reload_logic(&logic);
}


void reload_current_map()
{
  load_map(&map, cur_map);
  reload_logic(&logic);
}


void save_current_map()
{
  save_map(&map, cur_map);
}


void init(void)
{
  sg_setup(&(sg_desc){
      .context = sapp_sgcontext()
      });
  pass_action = (sg_pass_action) {
    .colors[0] = { .action=SG_ACTION_CLEAR, .val={0.5f, 0.5f, 0.5f, 1.0f } }
  };

  init_player(&player);
  init_map(&map);
  init_editor(&editor);

  run_map("level0");

  stm_setup();

  in.h = in.v = IN_NONE;
}


static void input(const sapp_event* ev)
{
  if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
    switch (ev->key_code) {
      case SAPP_KEYCODE_ESCAPE:
      case SAPP_KEYCODE_Q:
        sapp_quit();
        break;

      case SAPP_KEYCODE_W:
      case SAPP_KEYCODE_UP:
        in.v = IN_UP;
        break;

      case SAPP_KEYCODE_S:
      case SAPP_KEYCODE_DOWN:
        in.v = IN_DOWN;
        break;

      case SAPP_KEYCODE_A:
      case SAPP_KEYCODE_LEFT:
        in.h = IN_LEFT;
        break;

      case SAPP_KEYCODE_D:
      case SAPP_KEYCODE_RIGHT:
        in.h = IN_RIGHT;
        break;

      default:
        break;
    }
  } else if (ev->type == SAPP_EVENTTYPE_KEY_UP) {
    switch (ev->key_code) {
      case SAPP_KEYCODE_W:
      case SAPP_KEYCODE_UP:
        if (in.v == IN_UP) {
          in.v = IN_NONE;
        }
        break;

      case SAPP_KEYCODE_S:
      case SAPP_KEYCODE_DOWN:
        if (in.v == IN_DOWN) {
          in.v = IN_NONE;
        }
        break;

      case SAPP_KEYCODE_A:
      case SAPP_KEYCODE_LEFT:
        if (in.h == IN_LEFT) {
          in.h = IN_NONE;
        }
        break;

      case SAPP_KEYCODE_D:
      case SAPP_KEYCODE_RIGHT:
        if (in.h == IN_RIGHT) {
          in.h = IN_NONE;
        }
        break;

      case SAPP_KEYCODE_H:
        if (in_editor) {
          in_editor = false;
        } else {
          reload_current_map();
          in_editor = true;
        }
        break;

      case SAPP_KEYCODE_R:
        reload_current_map();
        break;

      case SAPP_KEYCODE_J:
        next_spot_type(&editor, &map);
        break;

      case SAPP_KEYCODE_N:
        kill_spot(&editor, &map);
        break;

      case SAPP_KEYCODE_P:
        save_current_map();
        break;

      default:
        break;
    }
  }
}


static uint64_t last_time = 0;
static const float dt = 1.0f / 60.0f;
static float t = 0.0f, frame_time, accumulator = 0.0f;
static float frame_fraction;

void frame(void)
{
  frame_time = stm_laptime(&last_time) / 1000000000.0f;
  accumulator += frame_time;

  while (accumulator >= dt) {
    if (!in_editor) {
      update(&player, t, dt, &in, &map, &logic);
    } else {
      update_editor(&editor, t, dt, &in, &map);
      in.v = in.h = IN_NONE;
    }
    accumulator -= dt;
    t += dt;
  }

  if (!in_editor) {
    frame_fraction = accumulator / dt;
  } else {
    frame_fraction = 0.0f;
  }


  sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());

  draw_map(&map, frame_fraction);
  draw_player(&player, frame_fraction);
  if (in_editor) {
    draw_editor(&editor);
  }

  sg_end_pass();

  sg_commit();
}


sapp_desc sokol_main(int argc, char* argv[]) {
  (void)argc; (void)argv;
  return (sapp_desc){
    .init_cb = init,
    .frame_cb = frame,
    .cleanup_cb = sg_shutdown,
    .event_cb = input,
    .width = 800,
    .height = 600,
    /* .fullscreen = true, */
    .high_dpi = true,
    .alpha = true,
    .gl_force_gles2 = true,
    .window_title = "Old",
  };
}
