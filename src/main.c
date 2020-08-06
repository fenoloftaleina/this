#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "main.glsl.h"


typedef struct buffer_object {
  sg_pass_action pass_action;
  sg_pipeline pip;
  sg_bindings bind;
  float* vertices;
  uint16_t* indices;
  int vertices_count;
  int indices_count;
} buffer_object;
static buffer_object player_bo;


void init_buffer_object(buffer_object* bo, int vertices_count, int indices_count)
{
  int vertices_size = vertices_count * sizeof(float);
  int indices_size = indices_count * sizeof(uint16_t);

  bo->bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
      .usage = SG_USAGE_DYNAMIC,
      .size = vertices_size
      });

  uint16_t indices[] = { 0, 1, 2,  0, 2, 3 };
  bo->bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
      .type = SG_BUFFERTYPE_INDEXBUFFER,
      .usage = SG_USAGE_DYNAMIC,
      .size = indices_size
      });

  sg_shader shd = sg_make_shader(main_shader_desc());

  bo->pip = sg_make_pipeline(&(sg_pipeline_desc){
      .shader = shd,
      .index_type = SG_INDEXTYPE_UINT16,
      .layout = {
        .attrs = {
          [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
          [ATTR_vs_color0].format   = SG_VERTEXFORMAT_FLOAT4
        }
      }
      });

  bo->pass_action = (sg_pass_action) {
    .colors[0] = { .action=SG_ACTION_CLEAR, .val={0.5f, 0.5f, 0.5f, 1.0f } }
  };

  bo->vertices = (float*)malloc(vertices_size);
  bo->indices = (uint16_t*)malloc(indices_size);
}


void update_buffer_object(buffer_object* bo, int vertices_count, int indices_count)
{
  sg_update_buffer(bo->bind.vertex_buffers[0], bo->vertices, vertices_count * sizeof(float));
  sg_update_buffer(bo->bind.index_buffer, bo->indices, indices_count * sizeof(uint16_t));
}


void init(void) {
  sg_setup(&(sg_desc){
      .context = sapp_sgcontext()
      });

  init_buffer_object(&player_bo, 28, 6);

  float vertices[] = {
    // positions            colors
    -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
    0.5f,  0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 0.0f, 1.0f,
  };
  uint16_t indices[] = { 0, 1, 2,  0, 2, 3 };

  memcpy(player_bo.vertices, vertices, sizeof(vertices));
  memcpy(player_bo.indices, indices, sizeof(indices));

  printf("%f %d - AAAAAAAAAA\n\n", player_bo.vertices[0], player_bo.indices[2]);

  update_buffer_object(&player_bo, 28, 6);
}


void draw_buffer_object(buffer_object* bo, int base_element, int num_elements, int num_instances)
{
  sg_begin_default_pass(&bo->pass_action, sapp_width(), sapp_height());
  sg_apply_pipeline(bo->pip);
  sg_apply_bindings(&bo->bind);
  sg_draw(base_element, num_elements, num_instances);
  sg_end_pass();
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
  draw_buffer_object(&player_bo, 0, 6, 1);
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
