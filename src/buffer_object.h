#include "stdlib.h"


typedef struct
{
  sg_pipeline pip;
  sg_bindings bind;
  float* vertices;
  uint16_t* indices;
  int vertices_count;
  int indices_count;
  int vertices_size;
  int indices_size;
} buffer_object;


const int vertices_per_rect = 4 * 7;
const int indices_per_rect = 2 * 3;
const int vertex_size = sizeof(float);
const int index_size = sizeof(uint16_t);

const float flat_z = 0.5f;


void set_buffer_counts
(buffer_object* bo, const int vertices_count, const int indices_count)
{
  bo->vertices_count = vertices_count;
  bo->indices_count = indices_count;
  bo->vertices_size = vertices_count * vertex_size;
  bo->indices_size = indices_count * index_size;
}


void init_buffer_object
(buffer_object* bo, const int vertices_count, const int indices_count)
{
  set_buffer_counts(bo, vertices_count, indices_count);

  sg_buffer_desc vertex_buffer_desc = {};
  vertex_buffer_desc.usage = SG_USAGE_DYNAMIC;
  vertex_buffer_desc.size = bo->vertices_size;
  bo->bind.vertex_buffers[0] = sg_make_buffer(&vertex_buffer_desc);

  sg_buffer_desc index_buffer_desc = {};
  index_buffer_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
  index_buffer_desc.usage = SG_USAGE_DYNAMIC;
  index_buffer_desc.size = bo->indices_size;
  bo->bind.index_buffer = sg_make_buffer(&index_buffer_desc);

  sg_shader shd = sg_make_shader(main_shader_desc());


  sg_pipeline_desc pip_desc = {};
  pip_desc.shader = shd;
  pip_desc.index_type = SG_INDEXTYPE_UINT16;

  sg_layout_desc layout_desc = {};

  sg_vertex_attr_desc vertex_position_desc = {};
  sg_vertex_attr_desc vertex_color0_desc = {};
  vertex_position_desc.format = SG_VERTEXFORMAT_FLOAT3;
  vertex_color0_desc.format = SG_VERTEXFORMAT_FLOAT4;

  layout_desc.attrs[ATTR_vs_position] = vertex_position_desc;
  layout_desc.attrs[ATTR_vs_color0] = vertex_color0_desc;

  pip_desc.layout = layout_desc;

  bo->pip = sg_make_pipeline(&pip_desc);


  bo->vertices = (float*)malloc(bo->vertices_size);
  bo->indices = (uint16_t*)malloc(bo->indices_size);
}


void update_buffer_vertices(buffer_object* bo)
{
  sg_update_buffer(bo->bind.vertex_buffers[0], bo->vertices, bo->vertices_size);
}


void update_buffer_indices(buffer_object* bo)
{
  sg_update_buffer(bo->bind.index_buffer, bo->indices, bo->indices_size);
}


void draw_buffer_object(const buffer_object* bo)
{
  if (bo->indices_count > 0) {
    sg_apply_pipeline(bo->pip);
    sg_apply_bindings(&bo->bind);
    sg_draw(0, bo->indices_count, 1);
  }
}
