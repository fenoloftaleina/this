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


static input_data in = {IN_NONE, IN_NONE};

static player_data player;
static map_data map;

static sg_pass_action pass_action;


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

  load_map(&map, "level0");

  stm_setup();

  in.h = in.v = IN_NONE;
}


static void input(const sapp_event* ev)
{
  if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
    switch (ev->key_code) {
      case SAPP_KEYCODE_ESCAPE:
        sapp_quit();
        break;

      case SAPP_KEYCODE_W:
      case SAPP_KEYCODE_UP:
        in.v = IN_UP;
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
      default:
        break;
    }
  }
}

static uint64_t last_time = 0;
static const float dt = 1.0f / 60.0f;
static float t = 0.0f, frame_time, accumulator = 0.0f;

void frame(void)
{
  frame_time = stm_laptime(&last_time) / 1000000000.0f;
  accumulator += frame_time;

  while (accumulator >= dt) {
    update(&player, t, dt, &in, &map);
    accumulator -= dt;
    t += dt;
  }

  const float frame_fraction = accumulator / dt;


  sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());

  draw_map(&map, frame_fraction);
  draw_player(&player, frame_fraction);

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
    .gl_force_gles2 = true,
    .window_title = "Old",
  };
}
