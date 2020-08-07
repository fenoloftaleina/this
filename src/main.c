#include "stdio.h"
#include "string.h"

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"

#include "main.glsl.h"

#include "buffer_object.h"
#include "rect.h"
#include "player.h"
#include "map.h"


static player_data player;
static map_data map;

static sg_pass_action pass_action;


void init(void) {
  sg_setup(&(sg_desc){
      .context = sapp_sgcontext()
      });
  pass_action = (sg_pass_action) {
    .colors[0] = { .action=SG_ACTION_CLEAR, .val={0.5f, 0.5f, 0.5f, 1.0f } }
  };

  init_player(&player);
  init_map(&map);

  init_level0(&map);
}


static void input(const sapp_event* ev) {
  if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
    switch (ev->key_code) {
      case SAPP_KEYCODE_ESCAPE:
        sapp_quit();
        break;

      case SAPP_KEYCODE_W:
      case SAPP_KEYCODE_UP:

        break;

      case SAPP_KEYCODE_A:
      case SAPP_KEYCODE_LEFT:

        break;

      case SAPP_KEYCODE_D:
      case SAPP_KEYCODE_RIGHT:

        break;
      default:
        break;
    }
  }
}


void frame(void) {
  sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());

  draw_player(&player);
  draw_map(&map);

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
