typedef struct rect {
  float x1, y1, x2, y2;
  float r, g, b, a;
} rect;


const float blend(const float a, const float b, const float f)
{
  return (1.0f - f) * a + f * b;
}


const int rects_write_vertices
(const rect* prs, const rect* rs, buffer_object* bo, const int count, const float f)
{
  for(int i = 0; i < count; ++i) {
    bo->vertices[i * vertices_per_rect +  0] = blend(prs[i].x1, rs[i].x1, f);
    bo->vertices[i * vertices_per_rect +  1] = blend(prs[i].y2, rs[i].y2, f);
    bo->vertices[i * vertices_per_rect +  2] = flat_z;
    bo->vertices[i * vertices_per_rect +  3] = blend(prs[i].r, rs[i].r, f);
    bo->vertices[i * vertices_per_rect +  4] = blend(prs[i].g, rs[i].g, f);
    bo->vertices[i * vertices_per_rect +  5] = blend(prs[i].b, rs[i].b, f);
    bo->vertices[i * vertices_per_rect +  6] = blend(prs[i].a, rs[i].a, f);

    bo->vertices[i * vertices_per_rect +  7] = blend(prs[i].x2, rs[i].x2, f);
    bo->vertices[i * vertices_per_rect +  8] = blend(prs[i].y2, rs[i].y2, f);
    bo->vertices[i * vertices_per_rect +  9] = flat_z;
    bo->vertices[i * vertices_per_rect + 10] = blend(prs[i].r, rs[i].r, f);
    bo->vertices[i * vertices_per_rect + 11] = blend(prs[i].g, rs[i].g, f);
    bo->vertices[i * vertices_per_rect + 12] = blend(prs[i].b, rs[i].b, f);
    bo->vertices[i * vertices_per_rect + 13] = blend(prs[i].a, rs[i].a, f);

    bo->vertices[i * vertices_per_rect + 14] = blend(prs[i].x2, rs[i].x2, f);
    bo->vertices[i * vertices_per_rect + 15] = blend(prs[i].y1, rs[i].y1, f);
    bo->vertices[i * vertices_per_rect + 16] = flat_z;
    bo->vertices[i * vertices_per_rect + 17] = blend(prs[i].r, rs[i].r, f);
    bo->vertices[i * vertices_per_rect + 18] = blend(prs[i].g, rs[i].g, f);
    bo->vertices[i * vertices_per_rect + 19] = blend(prs[i].b, rs[i].b, f);
    bo->vertices[i * vertices_per_rect + 20] = blend(prs[i].a, rs[i].a, f);

    bo->vertices[i * vertices_per_rect + 21] = blend(prs[i].x1, rs[i].x1, f);
    bo->vertices[i * vertices_per_rect + 22] = blend(prs[i].y1, rs[i].y1, f);
    bo->vertices[i * vertices_per_rect + 23] = flat_z;
    bo->vertices[i * vertices_per_rect + 24] = blend(prs[i].r, rs[i].r, f);
    bo->vertices[i * vertices_per_rect + 25] = blend(prs[i].g, rs[i].g, f);
    bo->vertices[i * vertices_per_rect + 26] = blend(prs[i].b, rs[i].b, f);
    bo->vertices[i * vertices_per_rect + 27] = blend(prs[i].a, rs[i].a, f);
  }

  return count * vertices_per_rect;
}


const int rects_write_vertices_simple(const rect* rs, buffer_object* bo, const int count)
{
  return rects_write_vertices(rs, rs, bo, count, 1.0f);
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


void move_rect(rect* r, const float x, const float y)
{
  r->x1 += x;
  r->y1 += y;
  r->x2 += x;
  r->y2 += y;
}
