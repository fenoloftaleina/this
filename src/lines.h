typedef struct
{
  parsl_context* ctx;
  int thickness;
} lines_data_t;

lines_data_t lines_data;


typedef struct
{
  float r, g, b, a;
} col_t;

void init_lines()
{
  lines_data.thickness = 1;
  lines_data.ctx = parsl_create_context((parsl_config){ .thickness = lines_data.thickness });
}


void add_lines_mesh
(buffer_object* bo, parsl_mesh* mesh, const col_t* color, const float z)
{
  int vertices_start = bo->vertices_count;
  int indices_start = bo->indices_count;

  for (uint32_t i = 0; i < mesh->num_vertices; ++i) {
    bo->vertices[(vertices_start + i) * vertex_elements_count +  0] = mesh->positions[i].x;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  1] = mesh->positions[i].y;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  2] = z;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  3] = color->r;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  4] = color->g;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  5] = color->b;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  6] = color->a;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  7] = -1.0f;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  8] = -1.0f;
  }

  for (uint32_t i = 0; i < mesh->num_triangles * 3; ++i) {
    bo->indices[indices_start + i] = vertices_start + mesh->triangle_indices[i];
  }


  inc_buffer_counts(bo, mesh->num_vertices, mesh->num_triangles * 3);
}


static uint16_t spine_lengths[1];

void add_lines
(buffer_object* bo,
 parsl_position* positions,
 const int count,
 const col_t* color,
 const float z,
 const bool closed)
{
  lines_data.ctx->config.thickness = lines_data.thickness;
  spine_lengths[0] = count;

  parsl_mesh* mesh = parsl_mesh_from_lines(lines_data.ctx, (parsl_spine_list){
      .num_vertices = count,
      .num_spines = 1,
      .vertices = positions,
      .spine_lengths = spine_lengths,
      .closed = closed
      });

  add_lines_mesh(bo, mesh, color, z);
}

void add_cubic_bezier
(buffer_object* bo,
 parsl_position* positions,
 const int count,
 const col_t* color,
 const float z,
 const bool closed)
{
  lines_data.ctx->config.thickness = lines_data.thickness;
  spine_lengths[0] = count;

  parsl_mesh* mesh = parsl_mesh_from_curves_cubic(lines_data.ctx, (parsl_spine_list){
      .num_vertices = count,
      .num_spines = 1,
      .vertices = positions,
      .spine_lengths = spine_lengths,
      .closed = closed
      });

  add_lines_mesh(bo, mesh, color, z);
}


void add_lines_rect
(buffer_object* bo,
 const float x1, const float y1, const float x2, const float y2,
 const col_t* color,
 const float z)
{
  add_lines(bo, (pos_t[]){{x1, y1}, {x1, y2}, {x2, y2}, {x2, y1}}, 4, color, z, true);
}


void introduce_lines_model
(parsl_position* positions,
 uint16_t* counts,
 int n,
 const bool closed)
{
  lines_data.ctx->config.thickness = lines_data.thickness;

  int full_count = 0;
  for (int i = 0; i < n; ++i) {
    full_count += counts[i];
  }

  parsl_mesh* mesh = parsl_mesh_from_lines(lines_data.ctx, (parsl_spine_list){
      .num_vertices = full_count,
      .num_spines = n,
      .vertices = positions,
      .spine_lengths = counts,
      .closed = closed
      });

  vertex_t* vertices = (vertex_t*)malloc(mesh->num_vertices * vertex_elements_count * vertex_size);
  index_t* indices = (index_t*)malloc(mesh->num_triangles * 3 * index_size);

  for (uint32_t i = 0; i < mesh->num_vertices; ++i) {
    vertices[i * vertex_elements_count + 0] = mesh->positions[i].x;
    vertices[i * vertex_elements_count + 1] = mesh->positions[i].y;
    vertices[i * vertex_elements_count + 2] = flat_z;
    vertices[i * vertex_elements_count + 3] = 0.0f;
    vertices[i * vertex_elements_count + 4] = 0.0f;
    vertices[i * vertex_elements_count + 5] = 0.0f;
    vertices[i * vertex_elements_count + 6] = 0.0f;
    vertices[i * vertex_elements_count + 7] = -1.0f;
    vertices[i * vertex_elements_count + 8] = -1.0f;
  }

  for (uint32_t i = 0; i < mesh->num_triangles * 3; ++i) {
    indices[i] = mesh->triangle_indices[i];
  }

  introduce_model(vertices, mesh->num_vertices, indices, mesh->num_triangles * 3);
  free(vertices);
  free(indices);
}
