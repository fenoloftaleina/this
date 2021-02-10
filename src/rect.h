typedef struct rect {
  float x1, y1, x2, y2;
  float r, g, b, a;
  float z;
  float u1, v1, u2, v2;
} rect;


const float blend(const float a, const float b, const float f)
{
  return (1.0f - f) * a + f * b;
}


void init_rects_buffer_object(buffer_object* bo, const int count)
{
  init_buffer_object(bo, count * 4, count * 6);
}


const int rects_write_vertices
(rect* prev_rects, const rect* rects, buffer_object* bo, const int count, const float f)
{
  float rounded_f = floor(f + 0.5f);

  for (int i = 0; i < count; ++i) {
    bo->vertices[i * 4 * vertex_elements_count +  0] = blend(prev_rects[i].x1, rects[i].x1, f);
    bo->vertices[i * 4 * vertex_elements_count +  1] = blend(prev_rects[i].y2, rects[i].y2, f);
    bo->vertices[i * 4 * vertex_elements_count +  2] = rects[i].z;
    bo->vertices[i * 4 * vertex_elements_count +  3] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[i * 4 * vertex_elements_count +  4] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[i * 4 * vertex_elements_count +  5] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[i * 4 * vertex_elements_count +  6] = blend(prev_rects[i].a, rects[i].a, f);
    bo->vertices[i * 4 * vertex_elements_count +  7] = blend(prev_rects[i].u1, rects[i].u1, rounded_f);
    bo->vertices[i * 4 * vertex_elements_count +  8] = blend(prev_rects[i].v2, rects[i].v2, rounded_f);

    bo->vertices[i * 4 * vertex_elements_count +  9] = blend(prev_rects[i].x2, rects[i].x2, f);
    bo->vertices[i * 4 * vertex_elements_count + 10] = blend(prev_rects[i].y2, rects[i].y2, f);
    bo->vertices[i * 4 * vertex_elements_count + 11] = rects[i].z;
    bo->vertices[i * 4 * vertex_elements_count + 12] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[i * 4 * vertex_elements_count + 13] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[i * 4 * vertex_elements_count + 14] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[i * 4 * vertex_elements_count + 15] = blend(prev_rects[i].a, rects[i].a, f);
    bo->vertices[i * 4 * vertex_elements_count + 16] = blend(prev_rects[i].u2, rects[i].u2, rounded_f);
    bo->vertices[i * 4 * vertex_elements_count + 17] = blend(prev_rects[i].v2, rects[i].v2, rounded_f);

    bo->vertices[i * 4 * vertex_elements_count + 18] = blend(prev_rects[i].x2, rects[i].x2, f);
    bo->vertices[i * 4 * vertex_elements_count + 19] = blend(prev_rects[i].y1, rects[i].y1, f);
    bo->vertices[i * 4 * vertex_elements_count + 20] = rects[i].z;
    bo->vertices[i * 4 * vertex_elements_count + 21] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[i * 4 * vertex_elements_count + 22] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[i * 4 * vertex_elements_count + 23] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[i * 4 * vertex_elements_count + 24] = blend(prev_rects[i].a, rects[i].a, f);
    bo->vertices[i * 4 * vertex_elements_count + 25] = blend(prev_rects[i].u2, rects[i].u2, rounded_f);
    bo->vertices[i * 4 * vertex_elements_count + 26] = blend(prev_rects[i].v1, rects[i].v1, rounded_f);

    bo->vertices[i * 4 * vertex_elements_count + 27] = blend(prev_rects[i].x1, rects[i].x1, f);
    bo->vertices[i * 4 * vertex_elements_count + 28] = blend(prev_rects[i].y1, rects[i].y1, f);
    bo->vertices[i * 4 * vertex_elements_count + 29] = rects[i].z;
    bo->vertices[i * 4 * vertex_elements_count + 30] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[i * 4 * vertex_elements_count + 31] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[i * 4 * vertex_elements_count + 32] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[i * 4 * vertex_elements_count + 33] = blend(prev_rects[i].a, rects[i].a, f);
    bo->vertices[i * 4 * vertex_elements_count + 34] = blend(prev_rects[i].u1, rects[i].u1, rounded_f);
    bo->vertices[i * 4 * vertex_elements_count + 35] = blend(prev_rects[i].v1, rects[i].v1, rounded_f);

    prev_rects[i] = rects[i];
  }

  return count * 4;
}


const int rects_write_vertices_simple(rect* rects, buffer_object* bo, const int count)
{
  return rects_write_vertices(rects, rects, bo, count, 1.0f);
}


const int rects_write_indices(buffer_object* bo, const int count)
{
  for(int i = 0; i < count; ++i) {
    bo->indices[i * 6 + 0] = i * 4 + 0;
    bo->indices[i * 6 + 1] = i * 4 + 1;
    bo->indices[i * 6 + 2] = i * 4 + 2;
    bo->indices[i * 6 + 3] = i * 4 + 0;
    bo->indices[i * 6 + 4] = i * 4 + 2;
    bo->indices[i * 6 + 5] = i * 4 + 3;
  }

  return count * 6;
}


void move_rect(rect* rect, const float x, const float y)
{
  rect->x1 += x;
  rect->y1 += y;
  rect->x2 += x;
  rect->y2 += y;
}


void init_rects(buffer_object* bo, const int count)
{
  init_rects_buffer_object(bo, count);
  bo->indices_count = rects_write_indices(bo, count);
  // update_buffer_indices(bo);
}


void draw_rects
(buffer_object* bo, rect* prev_rects, rect* rects, const int n, const float frame_fraction)
{
  set_buffer_counts(
    bo,
    rects_write_vertices(prev_rects, rects, bo, n, frame_fraction),
    n * 6
    );
  update_buffer_vertices(bo);

  draw_buffer_object(bo);
}


void set_sprite(rect* rect, texture_data* texture, const int sprite_id)
{
  rect->u1 = texture->mappings[sprite_id].x1;
  rect->v1 = texture->mappings[sprite_id].y1;
  rect->u2 = texture->mappings[sprite_id].x2;
  rect->v2 = texture->mappings[sprite_id].y2;
}


void add_rects
(buffer_object* bo, const rect* rects, rect* prev_rects, const int count, const float f)
{
  int vertices_start = bo->vertices_count;
  int indices_start = bo->indices_count;

  // printf("%d %d --- starts\n\n", vertices_start, indices_start);

  float rounded_f = floor(f + 0.5f);

  for (int i = 0; i < count; ++i) {
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  0] = blend(prev_rects[i].x1, rects[i].x1, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  1] = blend(prev_rects[i].y2, rects[i].y2, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  2] = rects[i].z;
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  3] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  4] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  5] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  6] = blend(prev_rects[i].a, rects[i].a, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  7] = blend(prev_rects[i].u1, rects[i].u1, rounded_f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  8] = blend(prev_rects[i].v2, rects[i].v2, rounded_f);

    bo->vertices[(vertices_start + i * 4) * vertex_elements_count +  9] = blend(prev_rects[i].x2, rects[i].x2, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 10] = blend(prev_rects[i].y2, rects[i].y2, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 11] = rects[i].z;
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 12] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 13] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 14] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 15] = blend(prev_rects[i].a, rects[i].a, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 16] = blend(prev_rects[i].u2, rects[i].u2, rounded_f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 17] = blend(prev_rects[i].v2, rects[i].v2, rounded_f);

    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 18] = blend(prev_rects[i].x2, rects[i].x2, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 19] = blend(prev_rects[i].y1, rects[i].y1, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 20] = rects[i].z;
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 21] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 22] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 23] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 24] = blend(prev_rects[i].a, rects[i].a, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 25] = blend(prev_rects[i].u2, rects[i].u2, rounded_f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 26] = blend(prev_rects[i].v1, rects[i].v1, rounded_f);

    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 27] = blend(prev_rects[i].x1, rects[i].x1, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 28] = blend(prev_rects[i].y1, rects[i].y1, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 29] = rects[i].z;
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 30] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 31] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 32] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 33] = blend(prev_rects[i].a, rects[i].a, f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 34] = blend(prev_rects[i].u1, rects[i].u1, rounded_f);
    bo->vertices[(vertices_start + i * 4) * vertex_elements_count + 35] = blend(prev_rects[i].v1, rects[i].v1, rounded_f);


    bo->indices[indices_start + i * 6 + 0] = vertices_start + i * 4 + 0;
    bo->indices[indices_start + i * 6 + 1] = vertices_start + i * 4 + 1;
    bo->indices[indices_start + i * 6 + 2] = vertices_start + i * 4 + 2;
    bo->indices[indices_start + i * 6 + 3] = vertices_start + i * 4 + 0;
    bo->indices[indices_start + i * 6 + 4] = vertices_start + i * 4 + 2;
    bo->indices[indices_start + i * 6 + 5] = vertices_start + i * 4 + 3;


    prev_rects[i] = rects[i];
  }

  inc_buffer_counts(bo, count * 4, count * 6);
}
