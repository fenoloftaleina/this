#include "stdlib.h"


typedef struct
{
  sg_pipeline pip;
  sg_bindings bind;
  float* vertices;
  uint16_t* indices;
  int vertices_count;
  int indices_count;
} buffer_object;


typedef float vertex_t;
typedef uint16_t index_t;
const int vertex_size = sizeof(vertex_t);
const int index_size = sizeof(index_t);

const int vertex_elements_count = 9;

const float flat_z = 1.0f;


void set_buffer_counts
(buffer_object* bo, const int vertices_count, const int indices_count)
{
  bo->vertices_count = vertices_count;
  bo->indices_count = indices_count;
}

void reset_buffer_counts(buffer_object* bo)
{
  set_buffer_counts(bo, 0, 0);
}

void inc_buffer_counts
(buffer_object* bo, const int inc_vertices_count, const int inc_indices_count)
{
  set_buffer_counts(
      bo,
      bo->vertices_count + inc_vertices_count,
      bo->indices_count + inc_indices_count
      );
}


void init_buffer_object
(buffer_object* bo, const int vertices_count, const int indices_count)
{
  set_buffer_counts(bo, vertices_count, indices_count);

  bo->bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
      .usage = SG_USAGE_DYNAMIC,
      .size = bo->vertices_count * vertex_size
      });

  bo->bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
      .type = SG_BUFFERTYPE_INDEXBUFFER,
      .usage = SG_USAGE_DYNAMIC,
      .size = bo->indices_count * index_size
      });

  sg_shader shd = sg_make_shader(main_shader_desc());

    uint32_t pixels[4*4] = {
        0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000,
        0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFF000000,
        0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF,
    };
    bo->bind.fs_images[SLOT_tex] = sg_make_image(&(sg_image_desc){
        .width = 4,
        .height = 4,
        .content.subimage[0][0] = {
            .ptr = pixels,
            .size = sizeof(pixels)
        },
        .label = "cube-texture"
    });

  bo->pip = sg_make_pipeline(&(sg_pipeline_desc){
      .shader = shd,
      .index_type = SG_INDEXTYPE_UINT16,
      .blend = {
        .enabled = true,
        .src_factor_rgb = SG_BLENDFACTOR_SRC_COLOR,
        .dst_factor_rgb = SG_BLENDFACTOR_DST_COLOR
      },
      .layout = {
        .attrs = {
          [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
          [ATTR_vs_color0].format   = SG_VERTEXFORMAT_FLOAT4,
          [ATTR_vs_texcoord0].format = SG_VERTEXFORMAT_FLOAT2
        }
      }
      });

  bo->vertices = (vertex_t*)malloc(bo->vertices_count * vertex_size);
  bo->indices = (index_t*)malloc(bo->indices_count * index_size);
}


void update_buffer_vertices(buffer_object* bo)
{
  sg_update_buffer(bo->bind.vertex_buffers[0], bo->vertices, bo->vertices_count * vertex_size);
}


void update_buffer_indices(buffer_object* bo)
{
  sg_update_buffer(bo->bind.index_buffer, bo->indices, bo->indices_count * index_size);
}


void draw_buffer_object(const buffer_object* bo)
{
  if (bo->indices_count > 0) {



    vs_params_t vs_params;
    const float w = (float) sapp_width();
    const float h = (float) sapp_height();
    hmm_mat4 proj = HMM_Orthographic(0.0f, w, 0.0f, h, 0.01f, 10.0f);
    hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 0.0f, 10.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
    vs_params.mvp = HMM_MultiplyMat4(proj, view);


    sg_apply_pipeline(bo->pip);
    sg_apply_bindings(&bo->bind);

    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &vs_params, sizeof(vs_params));

    sg_draw(0, bo->indices_count, 1);
  }
}
