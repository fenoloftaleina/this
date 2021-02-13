#include "stdio.h"
#include "string.h"

#include "cr.h"

static unsigned int CR_STATE version = 1;

#define SOKOL_DLL
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_time.h"
#include "sokol_glue.h"
#define HANDMADE_MATH_IMPLEMENTATION
#include "HandmadeMath.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui/cimgui.h"

#include "main.glsl.h"


const char* main_dir = "../../../main/";

const float dt = 1.0f / 60.0f;


#include "texture.h"
texture_data CR_STATE texture;


sg_shader CR_STATE main_shader;
sg_shader CR_STATE uv_frag_shader;


#include "buffer_object.h"
buffer_object CR_STATE rects_bo;
buffer_object CR_STATE sprites_bo;
buffer_object CR_STATE other_bo;
#include "rect.h"


static sg_pass_action CR_STATE pass_action;


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


static uint64_t CR_STATE last_time = 0;
static float CR_STATE t = 0.0f, frame_time, accumulator = 0.0f;
static float CR_STATE frame_fraction;

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


CR_EXPORT int cr_main(struct cr_plugin *ctx, enum cr_op operation)
{
  if (operation == CR_LOAD) {
    init();
  }

  if (operation != CR_STEP) {
    sg_shutdown();
    return 0;
  }

  // crash protection may cause the version to decrement. So we can test current version against one
  // tracked between instances with CR_STATE to signal that we're not running the most recent instance.
  if (ctx->version < version) {
    // a failure code is acessible in the `failure` variable from the `cr_plugin` context.
    // on windows this is the structured exception error code, for more info:
    //      https://msdn.microsoft.com/en-us/library/windows/desktop/ms679356(v=vs.85).aspx
    fprintf(stdout, "A rollback happened due to failure: %x!\n", ctx->failure);
  }
  version = ctx->version;

  // Not this does not carry state between instances (no CR_STATE), this means each time we load an instance
  // this value will be reset to its initial state (true), and then we can print the loaded instance version
  // one time only by instance version.
  static bool print_version = true;
  if (print_version) {
    fprintf(stdout, "loaded version: %d\n", ctx->version);

    // disable further printing for this instance only
    print_version = false;
  }
  frame();
  //std::this_thread::sleep_for(std::chrono::milliseconds(500));
  return 0;
}

/* sapp_desc sokol_main(int argc, char* argv[]) { */
/*   printf("HEREEEEEEEEEEEEEEEE!!!\n\n\n"); */
/* } */
