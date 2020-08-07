typedef struct rect {
  float x1, y1, x2, y2;
  float r, g, b, a;
} rect;


void rects_write_vertices(const rect* qs, buffer_object* bo, int count)
{
  for(int i = 0; i < count; ++i) {
    bo->vertices[i * vertices_per_rect +  0] = qs[i].x1;
    bo->vertices[i * vertices_per_rect +  1] = qs[i].y2;
    bo->vertices[i * vertices_per_rect +  2] = flat_z;
    bo->vertices[i * vertices_per_rect +  3] = qs[i].r;
    bo->vertices[i * vertices_per_rect +  4] = qs[i].g;
    bo->vertices[i * vertices_per_rect +  5] = qs[i].b;
    bo->vertices[i * vertices_per_rect +  6] = qs[i].a;

    bo->vertices[i * vertices_per_rect +  7] = qs[i].x2;
    bo->vertices[i * vertices_per_rect +  8] = qs[i].y2;
    bo->vertices[i * vertices_per_rect +  9] = flat_z;
    bo->vertices[i * vertices_per_rect + 10] = qs[i].r;
    bo->vertices[i * vertices_per_rect + 11] = qs[i].g;
    bo->vertices[i * vertices_per_rect + 12] = qs[i].b;
    bo->vertices[i * vertices_per_rect + 13] = qs[i].a;

    bo->vertices[i * vertices_per_rect + 14] = qs[i].x2;
    bo->vertices[i * vertices_per_rect + 15] = qs[i].y1;
    bo->vertices[i * vertices_per_rect + 16] = flat_z;
    bo->vertices[i * vertices_per_rect + 17] = qs[i].r;
    bo->vertices[i * vertices_per_rect + 18] = qs[i].g;
    bo->vertices[i * vertices_per_rect + 19] = qs[i].b;
    bo->vertices[i * vertices_per_rect + 20] = qs[i].a;

    bo->vertices[i * vertices_per_rect + 21] = qs[i].x1;
    bo->vertices[i * vertices_per_rect + 22] = qs[i].y1;
    bo->vertices[i * vertices_per_rect + 23] = flat_z;
    bo->vertices[i * vertices_per_rect + 24] = qs[i].r;
    bo->vertices[i * vertices_per_rect + 25] = qs[i].g;
    bo->vertices[i * vertices_per_rect + 26] = qs[i].b;
    bo->vertices[i * vertices_per_rect + 27] = qs[i].a;
  }
}


static uint16_t rect_indices[] = { 0, 1, 2,  0, 2, 3 };
static int rect_indices_size = sizeof(rect_indices);

void rects_write_indices(buffer_object* bo, int count)
{
  for(int i = 0; i < count; ++i) {
    memcpy(bo->indices + i * indices_per_rect, rect_indices, rect_indices_size);
  }
}
