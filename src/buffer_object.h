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
  reset_buffer_counts(bo);

  bo->bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
      .usage = SG_USAGE_DYNAMIC,
      .size = vertices_count * vertex_elements_count * vertex_size
      });

  bo->bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
      .type = SG_BUFFERTYPE_INDEXBUFFER,
      .usage = SG_USAGE_DYNAMIC,
      .size = indices_count * index_size
      });

  sg_shader shd = sg_make_shader(main_shader_desc());

  bo->bind.fs_images[SLOT_tex] = sg_alloc_image();

  bo->pip = sg_make_pipeline(&(sg_pipeline_desc){
      .shader = shd,
      .index_type = SG_INDEXTYPE_UINT16,
      .depth_stencil = {
        .depth_compare_func = SG_COMPAREFUNC_LESS,
        .depth_write_enabled = true,
      },
      .blend = {
        .enabled = true,
        .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
        .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        .op_rgb = SG_BLENDOP_ADD,
        .src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA,
        .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        .op_alpha = SG_BLENDOP_ADD
      },
      .layout = {
        .attrs = {
          [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
          [ATTR_vs_color0].format   = SG_VERTEXFORMAT_FLOAT4,
          [ATTR_vs_texcoord0].format = SG_VERTEXFORMAT_FLOAT2
        }
      }});

  bo->vertices = (vertex_t*)malloc(vertices_count * vertex_elements_count * vertex_size);
  bo->indices = (index_t*)malloc(indices_count * index_size);
}


void update_buffer_vertices(buffer_object* bo)
{
  sg_update_buffer(bo->bind.vertex_buffers[0], bo->vertices, bo->vertices_count * vertex_elements_count * vertex_size);
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

    // printf("\n\nbo indices %d vertices %d\n\n", bo->indices_count, bo->vertices_count);

    // for (int i = 0; i < bo->vertices_count; ++i) {
    //   printf("%f, ", bo->vertices[i]);
    // }

    sg_draw(0, bo->indices_count, 1);
  }
}


void put_in_buffer(
    buffer_object* bo,
    const vertex_t* new_vertices,
    const int new_vertices_count,
    index_t* new_indices,
    const int new_indices_count)
{
  const int vertices_start = bo->vertices_count;

  memcpy(
      bo->vertices + bo->vertices_count,
      new_vertices,
      new_vertices_count * vertex_elements_count * vertex_size);

  memcpy(
      bo->indices + bo->indices_count,
      new_indices,
      new_indices_count * index_size);

  for (int i = 0; i < new_indices_count; ++i) {
    bo->indices[bo->indices_count + i] += vertices_start;
  }

  inc_buffer_counts(
      bo,
      new_vertices_count,
      new_indices_count
      );
}


void set_texture(buffer_object* bo, texture_data* texture)
{
  sg_init_image(bo->bind.fs_images[SLOT_tex], &(sg_image_desc){
      .width = texture_size,
      .height = texture_size,
      .pixel_format = SG_PIXELFORMAT_RGBA8,
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
      .content.subimage[0][0] = {
          .ptr = texture->pixels,
          .size = texture_size * texture_size * 4
      }
  });

  free(texture->pixels);
}


void set_empty_texture(buffer_object* bo)
{
  uint32_t pixels[1] = {
    0xFFFFFFFF
  };
  bo->bind.fs_images[SLOT_tex] = sg_make_image(&(sg_image_desc){
      .width = 1,
      .height = 1,
      .content.subimage[0][0] = {
        .ptr = pixels,
        .size = sizeof(pixels)
      }});
}
