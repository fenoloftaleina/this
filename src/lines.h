typedef struct
{
  parsl_context* ctx;
  int thickness;
} lines_data_t;

lines_data_t CR_STATE lines_data;


typedef struct
{
  float r, g, b, a;
} col_t;

void init_lines()
{
  lines_data.thickness = 1;
  lines_data.ctx = parsl_create_context((parsl_config){ .thickness = lines_data.thickness });
}


void add_lines_mesh(buffer_object* bo, parsl_mesh* mesh, const col_t* color)
{
  int vertices_start = bo->vertices_count;
  int indices_start = bo->indices_count;

  for (uint32_t i = 0; i < mesh->num_vertices; ++i) {
    bo->vertices[(vertices_start + i) * vertex_elements_count +  0] = mesh->positions[i].x;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  1] = mesh->positions[i].y;
    bo->vertices[(vertices_start + i) * vertex_elements_count +  2] = flat_z;
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
 parsl_position* prev_positions,
 const int count,
 const col_t* color,
 const float frame_fraction)
{
  lines_data.ctx->config.thickness = lines_data.thickness;

  for (int i = 0; i < count; ++i) {
    // prev_positions used as temp, turned into new positions below
    prev_positions[i].x = lerp(prev_positions[i].x, positions[i].x, frame_fraction);
  }

  spine_lengths[0] = count;

  parsl_mesh* mesh = parsl_mesh_from_lines(lines_data.ctx, (parsl_spine_list){
      .num_vertices = count,
      .num_spines = 1,
      .vertices = prev_positions, // used as temp, see above
      .spine_lengths = spine_lengths,
      .closed = false
      });

  for (int i = 0; i < count; ++i) {
    prev_positions[i] = positions[i];
  }

  add_lines_mesh(bo, mesh, color);
}
