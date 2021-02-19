typedef struct
{
  vertex_t* vertices;
  index_t* indices;
  int* vertices_offsets;
  int* indices_offsets;
  int n;
} models_data_t;

models_data_t models_data;


void introduce_model
(vertex_t* vertices,
 int vertices_count,
 index_t* indices,
 int indices_count)
{
  memcpy(
      models_data.vertices + models_data.vertices_offsets[models_data.n],
      vertices,
      vertices_count * vertex_elements_count * vertex_size
      );
  memcpy(
      models_data.indices + models_data.indices_offsets[models_data.n],
      indices,
      indices_count * index_size
      );

  models_data.n += 1;
  models_data.vertices_offsets[models_data.n] =
    models_data.vertices_offsets[models_data.n - 1] + vertices_count * vertex_elements_count;
  models_data.indices_offsets[models_data.n] =
    models_data.indices_offsets[models_data.n - 1] + indices_count;
}


float to_rad(const float ang)
{
  return ang / 57.29578;
}

float rot_x(const float x, const float y, const float ang)
{
  return x * cos(ang) - y * sin(ang);
}

float rot_y(const float x, const float y, const float ang)
{
  return x * sin(ang) + y * cos(ang);
}


void init_models()
{
  models_data.vertices = (vertex_t*)malloc(100000 * vertex_size);
  models_data.indices = (index_t*)malloc(100000 * index_size);

  models_data.n = 0;
  models_data.vertices_offsets = (int*)malloc(50 * sizeof(int));
  models_data.indices_offsets = (int*)malloc(50 * sizeof(int));
  models_data.vertices_offsets[0] = 0;
  models_data.indices_offsets[0] = 0;
}


void add_models
(buffer_object* bo,
 const int* ids,
 const int n,
 const float scale,
 const rect* rects,
 rect* prev_rects,
 const float f)
{
  int temp_vertices_count, temp_indices_count, vertices_start, indices_start;

  for (int i = 0; i < n; ++i) {
    temp_vertices_count =
      models_data.vertices_offsets[ids[i] + 1] -
      models_data.vertices_offsets[ids[i]];
    temp_indices_count =
      models_data.indices_offsets[ids[i] + 1] -
      models_data.indices_offsets[ids[i]];

    vertices_start = bo->vertices_count;
    indices_start = bo->indices_count;

    for (int j = 0; j < temp_vertices_count; j += vertex_elements_count) {
      bo->vertices[vertices_start * vertex_elements_count + j + 0] =
        models_data.vertices[models_data.vertices_offsets[ids[i]] + j + 0] *
        scale +
        lerp(prev_rects[i].x1, rects[i].x1, f);
      bo->vertices[vertices_start * vertex_elements_count + j + 1] =
        models_data.vertices[models_data.vertices_offsets[ids[i]] + j + 1] *
        scale +
        lerp(prev_rects[i].y1, rects[i].y1, f);
      bo->vertices[vertices_start * vertex_elements_count + j + 2] =
        models_data.vertices[models_data.vertices_offsets[ids[i]] + j + 2] *
        scale +
        lerp(prev_rects[i].z, rects[i].z, f);
      bo->vertices[vertices_start * vertex_elements_count + j + 3] =
        models_data.vertices[models_data.vertices_offsets[ids[i]] + j + 3] +
        lerp(prev_rects[i].r, rects[i].r, f);
      bo->vertices[vertices_start * vertex_elements_count + j + 4] =
        models_data.vertices[models_data.vertices_offsets[ids[i]] + j + 4] +
        lerp(prev_rects[i].g, rects[i].g, f);
      bo->vertices[vertices_start * vertex_elements_count + j + 5] =
        models_data.vertices[models_data.vertices_offsets[ids[i]] + j + 5] +
        lerp(prev_rects[i].b, rects[i].b, f);
      bo->vertices[vertices_start * vertex_elements_count + j + 6] =
        models_data.vertices[models_data.vertices_offsets[ids[i]] + j + 6] +
        lerp(prev_rects[i].a, rects[i].a, f);
      bo->vertices[vertices_start * vertex_elements_count + j + 7] =
        models_data.vertices[models_data.vertices_offsets[ids[i]] + j + 7];
      bo->vertices[vertices_start * vertex_elements_count + j + 8] =
        models_data.vertices[models_data.vertices_offsets[ids[i]] + j + 8];
    }

    for (int j = 0; j < temp_indices_count; ++j) {
      bo->indices[indices_start + j] =
        models_data.indices[models_data.indices_offsets[ids[i]] + j] +
        vertices_start;
    }

    prev_rects[i] = rects[i];

    inc_buffer_counts(bo, temp_vertices_count, temp_indices_count);
  }
}
