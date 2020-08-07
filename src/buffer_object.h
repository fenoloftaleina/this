#include "stdlib.h"


typedef struct buffer_object {
  sg_pass_action pass_action;
  sg_pipeline pip;
  sg_bindings bind;
  float* vertices;
  uint16_t* indices;
  int vertices_count;
  int indices_count;
  int vertices_size;
  int indices_size;
} buffer_object;


const int vertices_per_quad = 4 * 7;
const int indices_per_quad = 2 * 3;
const int vertex_size = sizeof(float);
const int index_size = sizeof(uint16_t);

const float flat_z = 0.5f;


void init_buffer_object(buffer_object* bo, int vertices_count, int indices_count)
{
  bo->vertices_count = vertices_count;
  bo->indices_count = indices_count;
  bo->vertices_size = vertices_count * vertex_size;
  bo->indices_size = indices_count * index_size;

  bo->bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
      .usage = SG_USAGE_DYNAMIC,
      .size = bo->vertices_size
      });

  bo->bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
      .type = SG_BUFFERTYPE_INDEXBUFFER,
      .usage = SG_USAGE_DYNAMIC,
      .size = bo->indices_size
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

  bo->vertices = (float*)malloc(bo->vertices_size);
  bo->indices = (uint16_t*)malloc(bo->indices_size);
}


void update_buffer_object(buffer_object* bo)
{
  sg_update_buffer(bo->bind.vertex_buffers[0], bo->vertices, bo->vertices_size);
  sg_update_buffer(bo->bind.index_buffer, bo->indices, bo->indices_size);
}


void draw_buffer_object(buffer_object* bo)
{
  sg_begin_default_pass(&bo->pass_action, sapp_width(), sapp_height());
  sg_apply_pipeline(bo->pip);
  sg_apply_bindings(&bo->bind);
  sg_draw(0, bo->indices_count, 1);
  sg_end_pass();
}
