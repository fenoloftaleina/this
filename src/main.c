#include "stdio.h"
#include "string.h"

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"

#include "main.glsl.h"

#include "buffer_object.h"
#include "rect.h"


static buffer_object player_bo, map_bo;
static rect player_rect;



void init(void) {
  sg_setup(&(sg_desc){
      .context = sapp_sgcontext()
      });

  init_buffer_object(&player_bo, vertices_per_rect, indices_per_rect);
  init_buffer_object(&map_bo, vertices_per_rect, indices_per_rect);

  player_rect = (rect){
    -0.5f, -0.5f, 0.5f, 0.5f,
    0.8f, 0.6f, 0.7f
  };

  /* float vertices[] = { */
  /*   // positions            colors */
  /*   -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f, */
  /*   0.5f,  0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f, */
  /*   0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f, */
  /*   -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 0.0f, 1.0f, */
  /* }; */
  /* uint16_t indices[] = { 0, 1, 2,  0, 2, 3 }; */
  /*  */
  /* memcpy(player_bo.vertices, vertices, sizeof(vertices)); */
  /* memcpy(player_bo.indices, indices, sizeof(indices)); */

  rects_write_vertices(&player_rect, &player_bo, 1);
  rects_write_indices(&player_bo, 1);
  update_buffer_object(&player_bo);
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
  draw_buffer_object(&player_bo);
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
