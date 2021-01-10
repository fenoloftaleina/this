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


void add_pos_vertex(vertex_t* vertices, int i, float x, float y, float z)
{
  vertices[i + 0] = x;
  vertices[i + 1] = y;
  vertices[i + 2] = z;
  vertices[i + 3] = 0.0f;
  vertices[i + 4] = 0.0f;
  vertices[i + 5] = 0.0f;
  vertices[i + 6] = 0.0f;
  vertices[i + 7] = 0.0f;
  vertices[i + 8] = 0.0f;
}


void add_quad(
    vertex_t* vertices,
    int* vi,
    index_t* indices,
    int* ii,
    vertex_t* new_vertices,
    index_t* new_indices)
{
  memcpy(vertices + *vi, new_vertices, 4 * vertex_elements_count * sizeof(vertex_t));
  *vi = *vi + 4 * vertex_elements_count;
  memcpy(indices + *ii, new_indices, 6 * sizeof(index_t));
  *ii = *ii + 6;
}


bool potential_or_real_intersect
(const float x1, const float y1, const float x2, const float y2,
 const float x3, const float y3, const float x4, const float y4,
 float* x, float* y)
{
  float t, u;

  if ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4) == 0) {
    return false;
  } else {
    t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) /
      ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
    u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) /
      ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));

    *x = x1 + t * (x2 - x1);
    *y = y1 + t * (y2 - y1);

    return true;
  }
}


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
    0.0f,   120.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    100.0f, 120.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    100.0f, 0.0f,   1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
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


  vertex_t* bezier_vertices = (vertex_t*)malloc(1000 * vertex_size);
  index_t* bezier_indices = (index_t*)malloc(1000 * index_size);
  int bezier_vertices_count = 0;
  int bezier_indices_count = 0;


  hmm_vec2 line_points[100] = {
    0.0f, 15.0f,
    20.0f, 100.0f,
    100.0f, 200.0f,
    200.0f, 200.0f,
  };
  int line_points_count = 4;

  float half_width = 15.0f;

  hmm_vec2 XYs[100];
  int XYs_count = 0;

  for (int i = 1; i < line_points_count; ++i) {
    hmm_vec2 sub = HMM_SubtractVec2(
            line_points[i - 1],
            line_points[i]);
    hmm_vec2 cur_normalized_vec =
      HMM_NormalizeVec2(sub
          );

    hmm_vec2 perpendicular =
      HMM_MultiplyVec2f((hmm_vec2){
          cur_normalized_vec.Y * -1.0f,
          cur_normalized_vec.X
          }, half_width);

    int j = (i - 1) * 4;

    XYs[j + 0] = HMM_SubtractVec2(line_points[i - 1], perpendicular);
    XYs[j + 1] = HMM_AddVec2(line_points[i - 1], perpendicular);

    XYs[j + 2] = HMM_SubtractVec2(line_points[i], perpendicular);
    XYs[j + 3] = HMM_AddVec2(line_points[i], perpendicular);

    if (i > 1) {
      float x, y;

      if (potential_or_real_intersect(
            XYs[j - 4 + 0].X, XYs[j - 4 + 0].Y, XYs[j - 4 + 2].X, XYs[j - 4 + 2].Y,
            XYs[j + 0].X, XYs[j + 0].Y, XYs[j + 2].X, XYs[j + 2].Y,
            &x, &y)) {
        XYs[j - 4 + 2].X = x;
        XYs[j - 4 + 2].Y = y;
        XYs[j + 0].X = x;
        XYs[j + 0].Y = y;
      }

      if (potential_or_real_intersect(
            XYs[j - 4 + 1].X, XYs[j - 4 + 1].Y, XYs[j - 4 + 3].X, XYs[j - 4 + 3].Y,
            XYs[j + 1].X, XYs[j + 1].Y, XYs[j + 3].X, XYs[j + 3].Y,
            &x, &y)) {
        XYs[j - 4 + 3].X = x;
        XYs[j - 4 + 3].Y = y;
        XYs[j + 1].X = x;
        XYs[j + 1].Y = y;
      }
    }

    XYs_count += 4;

  }

  for (int i = 0; i < XYs_count; i += 4) {
    add_quad(
        bezier_vertices, &bezier_vertices_count,
        bezier_indices, &bezier_indices_count,
        (vertex_t[]){
        XYs[i + 1].X, XYs[i + 1].Y, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        XYs[i + 3].X, XYs[i + 3].Y, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        XYs[i + 2].X, XYs[i + 2].Y, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        XYs[i + 0].X, XYs[i + 0].Y, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
        },
        indices);

    for (int j = 0; j < 6; ++j) {
      indices[j] += 4;
    }
  }

  add_model(
      models,
      bezier_vertices,
      bezier_vertices_count,
      bezier_indices,
      bezier_indices_count
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

  int temp_vertices_count, temp_indices_count;

  for (int i = 0; i < n; ++i) {
    temp_vertices_count =
      models->vertices_offsets[ids[i] + 1] -
      models->vertices_offsets[ids[i]];
    temp_indices_count =
      models->indices_offsets[ids[i] + 1] -
      models->indices_offsets[ids[i]];

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

    memcpy(
        bo->indices + bo->indices_count,
        models->indices + models->indices_offsets[ids[i]],
        temp_indices_count * index_size
        );

    prev_rects[i] = rects[i];

    inc_buffer_counts(bo, temp_vertices_count, temp_indices_count);
  }

  update_buffer_vertices(bo);
  update_buffer_indices(bo);
}
