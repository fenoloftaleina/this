typedef struct
{
  vertex_t* vertices;
  index_t* indices;
  int* vertices_offsets;
  int* indices_offsets;
  int n;
} models_data;


void add_model
(models_data* models,
 vertex_t* vertices, int vertices_count,
 index_t* indices, int indices_count)
{
  memcpy(
      models->vertices + models->vertices_offsets[models->n],
      vertices,
      vertices_count * vertex_size
      );
  memcpy(
      models->indices + models->indices_offsets[models->n],
      indices,
      indices_count * index_size
      );

  models->n += 1;
  models->vertices_offsets[models->n] =
    models->vertices_offsets[models->n - 1] + vertices_count;
  models->indices_offsets[models->n] =
    models->indices_offsets[models->n - 1] + indices_count;
}


void import_model(models_data* models, const char* name)
{
}


const int vertex_model_elements_count = 3;


void init_models(models_data* models)
{
  models->vertices = (vertex_t*)malloc(100000 * vertex_size);
  models->indices = (index_t*)malloc(100000 * index_size);

  models->n = 0;
  models->vertices_offsets = (int*)malloc(50 * sizeof(int));
  models->indices_offsets = (int*)malloc(50 * sizeof(int));
  models->vertices_offsets[0] = 0;
  models->indices_offsets[0] = 0;


  // colors here for now, no tinting, no textures

  vertex_t vertices[] = {
    0.0f,   120.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f,
    100.0f, 120.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.1f,
    100.0f, 0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f,
    0.0f,   0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
  };
  index_t indices[] = {0, 1, 2, 0, 2, 3};

  add_model(
      models,
      vertices,
      4 * vertex_elements_count,
      indices,
      6
      );
}


void put_models_in_buffer
(const models_data* models, buffer_object* bo,
 const int n, const int* ids,
 const float scale,
 rect* prev_rects, const rect* rects, // TODO: change rects into pos/scale pair
 const float f)
{
  reset_buffer_counts(bo);

  int temp_vertices_count, temp_indices_count, temp_vertices_start = 0;

  for (int i = 0; i < n; ++i) {
    temp_vertices_count =
      models->vertices_offsets[ids[i] + 1] -
      models->vertices_offsets[ids[i]];
    temp_indices_count =
      models->indices_offsets[ids[i] + 1] -
      models->indices_offsets[ids[i]];

    temp_vertices_start = bo->vertices_count;

    for (int j = 0; j < temp_vertices_count; j += vertex_elements_count) {
      bo->vertices[bo->vertices_count + j + 0] =
        models->vertices[models->vertices_offsets[ids[i]] + j + 0] *
        scale +
        blend(prev_rects[i].x1, rects[i].x1, f);
      bo->vertices[bo->vertices_count + j + 1] =
        models->vertices[models->vertices_offsets[ids[i]] + j + 1] *
        scale +
        blend(prev_rects[i].y1, rects[i].y1, f);
      bo->vertices[bo->vertices_count + j + 2] =
        models->vertices[models->vertices_offsets[ids[i]] + j + 2] *
        scale;
      bo->vertices[bo->vertices_count + j + 3] =
        models->vertices[models->vertices_offsets[ids[i]] + j + 3] +
        blend(prev_rects[i].r, rects[i].r, f);
      bo->vertices[bo->vertices_count + j + 4] =
        models->vertices[models->vertices_offsets[ids[i]] + j + 4] +
        blend(prev_rects[i].g, rects[i].g, f);
      bo->vertices[bo->vertices_count + j + 5] =
        models->vertices[models->vertices_offsets[ids[i]] + j + 5] +
        blend(prev_rects[i].b, rects[i].b, f);
      bo->vertices[bo->vertices_count + j + 6] =
        models->vertices[models->vertices_offsets[ids[i]] + j + 6] +
        blend(prev_rects[i].a, rects[i].a, f);
      bo->vertices[bo->vertices_count + j + 7] =
        models->vertices[models->vertices_offsets[ids[i]] + j + 7];
      bo->vertices[bo->vertices_count + j + 8] =
        models->vertices[models->vertices_offsets[ids[i]] + j + 8];
    }

    for (int j = 0; j < temp_indices_count; ++j) {
      bo->indices[bo->indices_count + j] =
        models->indices[models->indices_offsets[ids[i]] + j] +
        temp_vertices_start;
    }

    prev_rects[i] = rects[i];

    inc_buffer_counts(bo, temp_vertices_count, temp_indices_count);
  }

  update_buffer_vertices(bo);
  update_buffer_indices(bo);
}
