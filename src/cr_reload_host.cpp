#include <cstdio>

#define CR_HOST CR_UNSAFE // try to best manage static states
#include "cr.h"

#define SOKOL_DLL
#include "sokol_app.h"

const char *plugin = CR_DEPLOY_PATH "/" CR_PLUGIN("guest");
cr_plugin ctx;


void init()
{
  cr_plugin_open(ctx, plugin);
}


void frame()
{
  cr_plugin_update(ctx);
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
    }
  }
}


void cleanup(void) {
  cr_plugin_close(ctx);
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
}
