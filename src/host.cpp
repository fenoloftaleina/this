#define CR_HOST CR_UNSAFE // try to best manage static states
#include "external/cr.h"

#include <cstdio>

#include "sokol_app.h"
#include "input_raw.h"

const char *plugin = CR_DEPLOY_PATH "/" CR_PLUGIN("main");
cr_plugin ctx;


static input_raw_t input_raw;


void init()
{
  printf("+ init\n");
  ctx.userdata = &input_raw;
  cr_plugin_open(ctx, plugin);
  fflush(stdout);
  fflush(stderr);
  printf("- init\n");
}


void frame()
{
  cr_plugin_update(ctx);
  if (input_raw.type == SAPP_EVENTTYPE_KEY_UP) {
    input_raw.type = SAPP_EVENTTYPE_INVALID;
  }
  fflush(stdout);
  fflush(stderr);
}


static void input(const sapp_event* ev)
{
  if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) {
    switch (ev->key_code) {
      case SAPP_KEYCODE_ESCAPE:
      case SAPP_KEYCODE_Q:
        sapp_quit();
        break;

      default:
        input_raw.type = ev->type;
        input_raw.key_code = ev->key_code;
    }
  } else if (ev->type == SAPP_EVENTTYPE_KEY_UP) {
    input_raw.type = ev->type;
    input_raw.key_code = ev->key_code;
  }
}


void cleanup(void) {
  printf("+ cleanup\n");
  cr_plugin_close(ctx);
  printf("- cleanup\n");
}


int main()
{
  sapp_desc desc;
  desc.init_cb = init;
  desc.frame_cb = frame;
  desc.cleanup_cb = cleanup;
  desc.event_cb = input;
  desc.fullscreen = true;
  desc.high_dpi = true;
  desc.alpha = true;
  desc.gl_force_gles2 = false;
  desc.window_title = "Old";

  sapp_run(&desc);

  return 0;
}
