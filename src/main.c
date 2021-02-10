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

/* #define RUN 0 */
/* #define RUN1 1 */
/* #define RUN_COUNT 2 */
/* #define SPRITES_COUNT 2 */

#include "schedule.h"
#include "tween.h"
#include "animation.h"

#include "texture.h"
texture_data texture;
#include "buffer_object.h"
buffer_object rects_bo;
buffer_object sprites_bo;
buffer_object other_bo;
#include "rect.h"

#include "map.h"
#include "in_types.h"
#include "death.h"
#include "player.h"
#include "logic.h"
#include "editor.h"
#include "input.h"

/* #include "generic.h" */


/* #define GUI */


// TODO: make something that makes spawning and using new BOs easier.
/* generic_data generic; */
/* generic_data generic2; */


static sg_pass_action pass_action;


/* #include "ments.h" */


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
    .colors[0] = { .action=SG_ACTION_CLEAR, .val={0.5f, 0.5f, 0.5f, 1.0f } }
  };


  init_buffer_object(&rects_bo, 40000, 60000);
  init_buffer_object(&sprites_bo, 40000, 60000);
  init_buffer_object(&other_bo, 40000, 60000);


  const int PATHS_COUNT = 7;
  const char* paths[PATHS_COUNT];
  paths[0] = "right0.png";
  paths[1] = "right1.png";
  paths[2] = "mondrian.png";
  paths[3] = "picasso.png";
  paths[4] = "rothko.png";
  paths[5] = "left0.png";
  paths[6] = "left1.png";
  init_texture(&texture, paths, PATHS_COUNT);


  init_player_animations();
  init_player();
  init_map();
  init_death();
  init_editor();


  set_empty_texture(&rects_bo);
  set_texture(&sprites_bo, &texture);
  set_empty_texture(&other_bo);

  /* init_generic(&generic, 5000000 * vertex_elements_count, 7000000); */
  /* set_empty_texture(&generic.bo); */
  /* init_generic(&generic2, 5000000 * vertex_elements_count, 7000000); */
  /* set_empty_texture(&generic2.bo); */
  /* init_ments(); */

#ifdef GUI
  simgui_setup(&(simgui_desc_t){ .dpi_scale = 2.0f });
#endif

  run_map("level0");

  stm_setup();

  in_data.h = in_data.v = IN_NONE;
  in_data.editor = false;
}


static void input(const sapp_event* ev)
{
  handle_input(ev);
}


static uint64_t last_time = 0;
static float t = 0.0f, frame_time, accumulator = 0.0f;
static float frame_fraction;

/* bool once = false; */

void frame(void)
{
  /* if (once) return; */
  /* once = true; */

  frame_time = stm_laptime(&last_time) / 1000000000.0f;
  accumulator += frame_time;

  while (accumulator >= dt) {
    if (!in_data.editor) {
      update_logic(t, dt);
      update_player(t);
      update_death(t);
    } else {
      update_editor(t, dt);
      in_data.v = in_data.h = IN_NONE;
    }
    accumulator -= dt;
    t += dt;
  }

  if (!in_data.editor) {
    frame_fraction = accumulator / dt;
  } else {
    frame_fraction = 0.0f;
  }


  sg_begin_default_pass(&pass_action, sapp_width(), sapp_height());

  reset_buffer_counts(&rects_bo);
  reset_buffer_counts(&sprites_bo);
  reset_buffer_counts(&other_bo);


  /* rect bg = { */
  /*   0.0f, 0.0f, 1000.0f, 1000.0f, */
  /*   1.0f, 1.0f, 1.0f, 1.0f, */
  /*   -1.0f, -1.0f, -1.0f, -1.0f, */
  /* }; */
  /* set_sprite(&bg, &texture, 2); */
  /* add_rects(&sprites_bo, &bg, &bg, 1, frame_fraction); */

  draw_player(frame_fraction);
  draw_map(frame_fraction);
  draw_death(frame_fraction);
  if (in_data.editor) {
    draw_editor(frame_fraction);
  }

  tick_buffer_object(&sprites_bo);
  tick_buffer_object(&rects_bo);
  tick_buffer_object(&other_bo);


  /* draw_ments(t); */
  /* draw_generic(&generic); */
  /* draw_generic(&generic2); */

  sdtx_draw();

#ifdef GUI
  simgui_new_frame(sapp_width(), sapp_height(), frame_time);

  igSetNextWindowPos((ImVec2){1200,20}, ImGuiCond_FirstUseEver, (ImVec2){0,0});

  igText("Hello, world!");

  igSliderFloat("gravity", &jump_state.gravity, -15000.0f, -5000.0f, "%.3f", 1.0f);
  igSliderFloat("init_v", &jump_state.init_v, 1000.0f, 4000.0f, "%.3f", 1.0f);
  igSliderFloat("init_double_v", &jump_state.init_double_v, 1000.0f, 4000.0f, "%.3f", 1.0f);
  igSliderFloat("low_clamp", &jump_state.low_clamp, -10000.0f, -5000.0f, "%.3f", 1.0f);
  igSliderFloat("high_clamp", &jump_state.low_clamp, 1000.0f, 5000.0f, "%.3f", 1.0f);

  igSliderFloat("default_clamp", &walk_state.default_clamp, 50.0f, 3000.0f, "%.3f", 1.0f);
  igSliderFloat("flight_clamp", &walk_state.flight_clamp, 50.0f, 3000.0f, "%.3f", 1.0f);
  igSliderFloat("damping", &walk_state.damping, 2000.0f, 40000.0f, "%.3f", 1.0f);
  igSliderFloat("transpose", &walk_state.damping, 2000.0f, 20000.0f, "%.3f", 1.0f);

  simgui_render();
#endif

  sg_end_pass();

  sg_commit();
}


void cleanup(void) {
#ifdef GUI
  simgui_shutdown();
#endif
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
