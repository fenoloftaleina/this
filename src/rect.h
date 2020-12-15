typedef struct rect {
  float x1, y1, x2, y2;
  float r, g, b, a;
} rect;


const float blend(const float a, const float b, const float f)
{
  return (1.0f - f) * a + f * b;
}


void init_rects_buffer_object(buffer_object* bo, const int count)
{
  init_buffer_object(bo, count * vertices_per_rect, count * indices_per_rect);
}


const int rects_write_vertices
(rect* prev_rects, const rect* rects, buffer_object* bo, const int count, const float f)
{
  for(int i = 0; i < count; ++i) {
    bo->vertices[i * vertices_per_rect +  0] = blend(prev_rects[i].x1, rects[i].x1, f);
    bo->vertices[i * vertices_per_rect +  1] = blend(prev_rects[i].y2, rects[i].y2, f);
    bo->vertices[i * vertices_per_rect +  2] = flat_z;
    bo->vertices[i * vertices_per_rect +  3] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[i * vertices_per_rect +  4] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[i * vertices_per_rect +  5] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[i * vertices_per_rect +  6] = blend(prev_rects[i].a, rects[i].a, f);

    bo->vertices[i * vertices_per_rect +  7] = blend(prev_rects[i].x2, rects[i].x2, f);
    bo->vertices[i * vertices_per_rect +  8] = blend(prev_rects[i].y2, rects[i].y2, f);
    bo->vertices[i * vertices_per_rect +  9] = flat_z;
    bo->vertices[i * vertices_per_rect + 10] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[i * vertices_per_rect + 11] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[i * vertices_per_rect + 12] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[i * vertices_per_rect + 13] = blend(prev_rects[i].a, rects[i].a, f);

    bo->vertices[i * vertices_per_rect + 14] = blend(prev_rects[i].x2, rects[i].x2, f);
    bo->vertices[i * vertices_per_rect + 15] = blend(prev_rects[i].y1, rects[i].y1, f);
    bo->vertices[i * vertices_per_rect + 16] = flat_z;
    bo->vertices[i * vertices_per_rect + 17] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[i * vertices_per_rect + 18] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[i * vertices_per_rect + 19] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[i * vertices_per_rect + 20] = blend(prev_rects[i].a, rects[i].a, f);

    bo->vertices[i * vertices_per_rect + 21] = blend(prev_rects[i].x1, rects[i].x1, f);
    bo->vertices[i * vertices_per_rect + 22] = blend(prev_rects[i].y1, rects[i].y1, f);
    bo->vertices[i * vertices_per_rect + 23] = flat_z;
    bo->vertices[i * vertices_per_rect + 24] = blend(prev_rects[i].r, rects[i].r, f);
    bo->vertices[i * vertices_per_rect + 25] = blend(prev_rects[i].g, rects[i].g, f);
    bo->vertices[i * vertices_per_rect + 26] = blend(prev_rects[i].b, rects[i].b, f);
    bo->vertices[i * vertices_per_rect + 27] = blend(prev_rects[i].a, rects[i].a, f);

    prev_rects[i] = rects[i];
  }

  return count * vertices_per_rect;
}


const int rects_write_vertices_simple(rect* rects, buffer_object* bo, const int count)
{
  return rects_write_vertices(rects, rects, bo, count, 1.0f);
}


const int rects_write_indices(buffer_object* bo, const int count)
{
  for(int i = 0; i < count; ++i) {
    bo->indices[i * indices_per_rect + 0] = i * 4 + 0;
    bo->indices[i * indices_per_rect + 1] = i * 4 + 1;
    bo->indices[i * indices_per_rect + 2] = i * 4 + 2;
    bo->indices[i * indices_per_rect + 3] = i * 4 + 0;
    bo->indices[i * indices_per_rect + 4] = i * 4 + 2;
    bo->indices[i * indices_per_rect + 5] = i * 4 + 3;
  }

  return count * indices_per_rect;
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
  rects_write_indices(bo, count);
  update_buffer_indices(bo);
}


void draw_rects
(buffer_object* bo, rect* prev_rects, rect* rects, const int n, const float frame_fraction)
{
  set_buffer_counts(
    bo,
    rects_write_vertices(prev_rects, rects, bo, n, frame_fraction),
    n * indices_per_rect
    );
  update_buffer_vertices(bo);

  draw_buffer_object(bo);
}
