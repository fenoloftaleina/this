#include "stdio.h"
#include "string.h"

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_time.h"
#define SOKOL_DEBUGTEXT_IMPL
#include "sokol_debugtext.h"
#define HANDMADE_MATH_IMPLEMENTATION
#include "HandmadeMath.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"
#define SOKOL_IMGUI_IMPL
#include "sokol_imgui.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "main.glsl.h"


const char* main_dir = "../../../main/";

const float dt = 1.0f / 60.0f;


#include "texture.h"
texture_data texture;


sg_shader main_shader;
sg_shader uv_frag_shader;


#include "buffer_object.h"
buffer_object rects_bo;
buffer_object sprites_bo;
buffer_object other_bo;
#include "rect.h"


static sg_pass_action pass_action;


void init(void)
{
  sg_setup(&(sg_desc){
      .context = sapp_sgcontext()
      });

  sdtx_setup(&(sdtx_desc_t){
    .fonts = {
      [0] = sdtx_font_kc854()
    },
  });

  sdtx_canvas(sapp_width() * 0.2f, sapp_height() * 0.2f);
  sdtx_origin(3.0f, 3.0f);
  sdtx_font(0);
  sdtx_color3f(1.0f, 0.0f, 0.0f);

  pass_action = (sg_pass_action) {
    .colors[0] = { .action=SG_ACTION_CLEAR, .val={0.8f, 0.8f, 0.8f, 1.0f } }
  };


  main_shader = sg_make_shader(main_shader_desc());
  uv_frag_shader = sg_make_shader(uv_frag_shader_desc());

  init_buffer_object(&rects_bo, 40000, 60000, &main_shader);
  init_buffer_object(&sprites_bo, 40000, 60000, &main_shader);
  init_buffer_object(&other_bo, 40000, 60000, &uv_frag_shader);


  const int PATHS_COUNT = 9;
  const char* paths[PATHS_COUNT];
  paths[0] = "mondrian.png";
  paths[1] = "picasso.png";
  paths[2] = "rothko.png";
  paths[3] = "mondrian.png";
  paths[4] = "picasso.png";
  paths[5] = "right0.png";
  paths[6] = "right1.png";
  paths[7] = "left0.png";
  paths[8] = "left1.png";
  init_texture(&texture, paths, PATHS_COUNT);

  set_empty_texture(&rects_bo);
  set_texture(&sprites_bo, &texture);
  set_empty_texture(&other_bo);


  stm_setup();
}


static void input(const sapp_event* ev)
{
  if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
    switch (ev->key_code) {
      case SAPP_KEYCODE_ESCAPE:
      case SAPP_KEYCODE_Q:
        sapp_quit();
        break;
    }
  }
}


static uint64_t last_time = 0;
static float t = 0.0f, frame_time, accumulator = 0.0f;
static float frame_fraction;

void frame(void)
{
  frame_time = stm_laptime(&last_time) / 1000000000.0f;
  accumulator += frame_time;

  while (accumulator >= dt) {

    accumulator -= dt;
    t += dt;
  }

  frame_fraction = accumulator / dt;


  sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());

  reset_buffer_counts(&rects_bo);
  reset_buffer_counts(&sprites_bo);
  reset_buffer_counts(&other_bo);


  rect bg = {
    0.0f, 0.0f, 1000.0f, 1000.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    flat_z,
    -1.0f, -1.0f, -1.0f, -1.0f,
  };
  set_sprite(&bg, &texture, 2);
  add_rects(&sprites_bo, &bg, &bg, 1, frame_fraction);


  rect r = (rect){2300.0f, 1100.0f, 2800.0f, 1600.0f, 0.5f, 0.5f, 0.5f, 1.0f, flat_z + 0.5f, 0.0f, 0.0f, 1.0f, 1.0f};
  add_rects(&other_bo, &r, &r, 1, frame_fraction);
  /* other_bo.indices_count = 3; */



  tick_buffer_object(&sprites_bo);
  tick_buffer_object(&rects_bo);
  tick_buffer_object(&other_bo);

  sdtx_draw();

  sg_end_pass();

  sg_commit();
}


void cleanup(void) {
  sg_shutdown();
}


sapp_desc sokol_main(int argc, char* argv[]) {
  (void)argc; (void)argv;
  return (sapp_desc){
    .init_cb = init,
    .frame_cb = frame,
    .cleanup_cb = cleanup,
    .event_cb = input,
    /* .width = 800, */
    /* .height = 600, */
    .fullscreen = true,
    .high_dpi = true,
    .alpha = true,
    .gl_force_gles2 = false,
    .window_title = "Old",
  };
}
