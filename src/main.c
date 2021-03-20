#include "stdio.h"
#include "string.h"

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_time.h"
#define SOKOL_DEBUGTEXT_IMPL
#include "sokol_debugtext.h"
#define HANDMADE_MATH_IMPLEMENTATION
#include "external/HandmadeMath.h"
#define PAR_STREAMLINES_IMPLEMENTATION
#include "external/par_streamlines.h"
typedef parsl_position pos_t;
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"
#define SOKOL_IMGUI_IMPL
#include "sokol_imgui.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define MINIAUDIO_IMPLEMENTATION
#include "external/miniaudio.h"

#include "main.glsl.h"


/* #define GUI */


const char* main_dir = "../../../main/";

const float dt = 1.0f / 60.0f;

uint64_t last_time = 0;
float t = 0.0f, frame_time, accumulator = 0.0f;
float frame_fraction;


/* #define RUN 0 */
/* #define RUN1 1 */
/* #define RUN_COUNT 2 */
/* #define SPRITES_COUNT 2 */


#include "schedule.h"
#include "tween.h"
#include "animation.h"

#include "texture.h"
texture_data texture;


sg_shader main_shader;
sg_shader uv_frag_shader;
sg_shader death_shader;


#include "buffer_object.h"
buffer_object rects_bo;
buffer_object sprites_bo;
buffer_object other_bo;
buffer_object lines_bo;
buffer_object death_bo;
#include "rect.h"
#include "models.h"
#include "lines.h"

#include "rect_animation.h"

#include "map.h"
#include "in_types.h"
#include "death.h"
#include "player.h"

#include "audio.h"

#include "logic.h"
#include "editor.h"
#include "map_list.h"
#include "input.h"

/* #include "generic.h" */



// TODO: make something that makes spawning and using new BOs easier.
/* generic_data generic; */
/* generic_data generic2; */


static sg_pass_action pass_action;


/* #include "ments.h" */

static const float white_f = 0.9f;

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

  sdtx_canvas(sapp_width() * 0.05f, sapp_height() * 0.05f);
  sdtx_origin(3.0f, 3.0f);
  sdtx_font(0);
  sdtx_color3f(1.0f, 0.0f, 0.0f);

  pass_action = (sg_pass_action) {
    .colors[0] = { .action=SG_ACTION_CLEAR, .val={white_f, white_f, white_f, 1.0f } }
  };


  main_shader = sg_make_shader(main_shader_desc());
  uv_frag_shader = sg_make_shader(uv_frag_shader_desc());
  death_shader = sg_make_shader(death_shader_desc());

  init_buffer_object(&rects_bo, 40000, 60000, &main_shader);
  init_buffer_object(&sprites_bo, 40000, 60000, &main_shader);
  init_buffer_object(&other_bo, 40000, 60000, &uv_frag_shader);
  init_buffer_object(&lines_bo, 40000, 60000, &main_shader);
  init_buffer_object(&death_bo, 40000, 60000, &death_shader);


  const int PATHS_COUNT = 6;
  const char* paths[PATHS_COUNT];
  paths[0] = "czekoladka.png";
  paths[1] = "czekoladka2.png";
  paths[2] = "kupa.png";
  paths[3] = "kupa2.png";
  paths[4] = "gracz.png";
  paths[5] = "gracz2.png";
  init_texture(&texture, paths, PATHS_COUNT);

  init_models();
  init_lines();


  float model_half_width = 60.0f * 0.5f;
  float model_mid = 100.0f;
  float a1 = - model_half_width;
  float a2 = model_half_width;
  float ang = to_rad(45.0f);
  float x1 = model_mid + rot_x(a1, a2, ang);
  float y1 = model_mid + rot_y(a1, a2, ang);
  float x2 = model_mid + rot_x(a2, a2, ang);
  float y2 = model_mid + rot_y(a2, a2, ang);
  float x3 = model_mid + rot_x(a2, a1, ang);
  float y3 = model_mid + rot_y(a2, a1, ang);
  float x4 = model_mid + rot_x(a1, a1, ang);
  float y4 = model_mid + rot_y(a1, a1, ang);
  vertex_t vertices[] = {
    x1, y1, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f,
    x2, y2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f,
    x3, y3, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f,
    x4, y4, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f
  };
  index_t indices[] = {0, 1, 2, 0, 2, 3};

  introduce_model(
      vertices,
      4,
      indices,
      6
      );


  float thickness = 4.0f;
  lines_data.thickness = (int)thickness;
  float beginning = 0.0f;
  float frame_size = 200.0f;
  float inset = 25.0f;
  introduce_lines_model(
      (pos_t[]){
        {beginning, beginning},
        {frame_size, beginning},
        {frame_size, frame_size},
        {beginning, frame_size},
        {beginning, beginning - thickness * 0.5f},

        {beginning, beginning},
        {inset, inset},

        {beginning, frame_size},
        {inset, frame_size - inset},

        {frame_size, frame_size},
        {frame_size - inset, frame_size - inset},

        {frame_size, beginning},
        {frame_size - inset, inset},

        {inset, inset},
        {frame_size - inset, inset},
        {frame_size - inset, frame_size - inset},
        {inset, frame_size - inset},
        {inset, inset - thickness * 0.5f},
      },
      (uint16_t[]){5, 2, 2, 2, 2, 5},
      6,
      false);

  introduce_lines_model(
      (pos_t[]){
        {x1, y1},
        {x2, y2},
        {x3, y3},
        {x4, y4},
        {x1, y1},
      },
      (uint16_t[]){5},
      1,
      false);

  vertex_t vertices_checkpoint[] = {
    0.0f, tile_height, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f,
    tile_width, tile_height, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f,
    tile_width, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f,
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f
  };
  index_t indices_checkpoint[] = {0, 1, 2, 0, 2, 3};

  introduce_model(
      vertices_checkpoint,
      4,
      indices_checkpoint,
      6
      );

  init_audio();

  init_map();
  init_player();
  init_paths();
  init_death();
  init_editor();


  set_empty_texture(&rects_bo);
  set_texture(&sprites_bo, &texture);
  set_empty_texture(&other_bo);
  set_empty_texture(&lines_bo);
  set_empty_texture(&death_bo);

  /* init_generic(&generic, 5000000 * vertex_elements_count, 7000000); */
  /* set_empty_texture(&generic.bo); */
  /* init_generic(&generic2, 5000000 * vertex_elements_count, 7000000); */
  /* set_empty_texture(&generic2.bo); */
  /* init_ments(); */

#ifdef GUI
  simgui_setup(&(simgui_desc_t){ .dpi_scale = 2.0f });
#endif

  load_map_list();

  run_map(0);
  /* for (int i = 0; i < 8; ++i) { */
  /*   strcpy(cur_map_name, map_list[i]); */
  /*   load_map(cur_map_name); */
  /*   save_map(cur_map_name); */
  /* } */

  stm_setup();

  in_data.h = in_data.v = IN_NONE;
  in_data.editor = false;
}


static void input(const sapp_event* ev)
{
  handle_input(ev, t);
}


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
      update_map(t);
      update_death(t);

      if (player_data.won && cur_map_i + 1 < map_list_n) {
        play_audio("wygrana.wav");
        run_map(cur_map_i + 1);
      }

      if (player_data.lost) {
        reload_current_map();
      }
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
  reset_buffer_counts(&lines_bo);
  reset_buffer_counts(&death_bo);


  /* rect bg = { */
  /*   0.0f, 0.0f, 1000.0f, 1000.0f, */
  /*   1.0f, 1.0f, 1.0f, 1.0f, */
  /*   -1.0f, -1.0f, -1.0f, -1.0f, */
  /* }; */
  /* set_sprite(&bg, &texture, 2); */
  /* add_rects(&sprites_bo, &bg, &bg, 1, frame_fraction); */

  draw_player(frame_fraction);
  /* draw_logic(frame_fraction); */
  draw_map(frame_fraction);
  /* draw_paths(); */
  /* draw_death(frame_fraction); */
  if (in_data.editor) {
    draw_editor(frame_fraction);
  }


  // uv rect outline via shader
  /* rect r = (rect){2300.0f, 1100.0f, 2800.0f, 1600.0f, 0.5f, 0.5f, 0.5f, 1.0f, flat_z + 0.5f, 0.0f, 0.0f, 1.0f, 1.0f}; */
  /* add_rects(&other_bo, &r, &r, 1, frame_fraction); */


  /* lines_data.thickness = 10; */
  /* pos_t positions[] = { {100, 100}, {300, 200}, {500, 100} }; */
  /* add_lines(&lines_bo, positions, 3, &(col_t){0.1f, 0.1f, 0.1f, 1.0f}, flat_z, false); */


  tick_buffer_object(&sprites_bo);
  tick_buffer_object(&rects_bo);
  tick_buffer_object(&other_bo);
  tick_buffer_object(&lines_bo);
  tick_buffer_object(&death_bo);


  /* draw_ments(t); */
  /* draw_generic(&generic); */
  /* draw_generic(&generic2); */

  sdtx_draw();

#ifdef GUI
  simgui_new_frame(sapp_width(), sapp_height(), frame_time);

  igSetNextWindowPos((ImVec2){1200,20}, ImGuiCond_FirstUseEver, (ImVec2){0,0});

  /* igText("Hello, world!"); */

  /* igSliderFloat("gravity", &jump_state.gravity, -15000.0f, -5000.0f, "%.3f", 1.0f); */
  /* igSliderFloat("init_v", &jump_state.init_v, 1000.0f, 4000.0f, "%.3f", 1.0f); */
  /* igSliderFloat("init_double_v", &jump_state.init_double_v, 1000.0f, 4000.0f, "%.3f", 1.0f); */
  /* igSliderFloat("low_clamp", &jump_state.low_clamp, -10000.0f, -5000.0f, "%.3f", 1.0f); */
  /* igSliderFloat("high_clamp", &jump_state.low_clamp, 1000.0f, 5000.0f, "%.3f", 1.0f); */
  /*  */
  /* igSliderFloat("default_clamp", &walk_state.default_clamp, 50.0f, 3000.0f, "%.3f", 1.0f); */
  /* igSliderFloat("flight_clamp", &walk_state.flight_clamp, 50.0f, 3000.0f, "%.3f", 1.0f); */
  /* igSliderFloat("damping", &walk_state.damping, 2000.0f, 40000.0f, "%.3f", 1.0f); */
  /* igSliderFloat("transpose", &walk_state.damping, 2000.0f, 20000.0f, "%.3f", 1.0f); */

  igColorEdit3("type colors 1", &type_colors[1], ImGuiColorEditFlags_Float);
  igColorEdit3("type colors 2", &type_colors[2], ImGuiColorEditFlags_Float);
  igColorEdit3("type colors 3", &type_colors[3], ImGuiColorEditFlags_Float);
  igColorEdit3("type colors 4", &type_colors[4], ImGuiColorEditFlags_Float);
  igColorEdit3("type colors 5", &type_colors[5], ImGuiColorEditFlags_Float);

  simgui_render();
#endif

  sg_end_pass();

  sg_commit();
}


void cleanup(void) {
#ifdef GUI
  simgui_shutdown();
#endif
  /* parsl_destroy_context(lines_data.ctx); */
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
