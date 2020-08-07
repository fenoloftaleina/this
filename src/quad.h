typedef struct quad {
  float x1, y1, x2, y2;
  float r, g, b, a;
} quad;


void quads_write_vertices(const quad* qs, buffer_object* bo, int count)
{
  for(int i = 0; i < count; ++i) {
    bo->vertices[i * vertices_per_quad +  0] = qs[i].x1;
    bo->vertices[i * vertices_per_quad +  1] = qs[i].y2;
    bo->vertices[i * vertices_per_quad +  2] = flat_z;
    bo->vertices[i * vertices_per_quad +  3] = qs[i].r;
    bo->vertices[i * vertices_per_quad +  4] = qs[i].g;
    bo->vertices[i * vertices_per_quad +  5] = qs[i].b;
    bo->vertices[i * vertices_per_quad +  6] = qs[i].a;

    bo->vertices[i * vertices_per_quad +  7] = qs[i].x2;
    bo->vertices[i * vertices_per_quad +  8] = qs[i].y2;
    bo->vertices[i * vertices_per_quad +  9] = flat_z;
    bo->vertices[i * vertices_per_quad + 10] = qs[i].r;
    bo->vertices[i * vertices_per_quad + 11] = qs[i].g;
    bo->vertices[i * vertices_per_quad + 12] = qs[i].b;
    bo->vertices[i * vertices_per_quad + 13] = qs[i].a;

    bo->vertices[i * vertices_per_quad + 14] = qs[i].x2;
    bo->vertices[i * vertices_per_quad + 15] = qs[i].y1;
    bo->vertices[i * vertices_per_quad + 16] = flat_z;
    bo->vertices[i * vertices_per_quad + 17] = qs[i].r;
    bo->vertices[i * vertices_per_quad + 18] = qs[i].g;
    bo->vertices[i * vertices_per_quad + 19] = qs[i].b;
    bo->vertices[i * vertices_per_quad + 20] = qs[i].a;

    bo->vertices[i * vertices_per_quad + 21] = qs[i].x1;
    bo->vertices[i * vertices_per_quad + 22] = qs[i].y1;
    bo->vertices[i * vertices_per_quad + 23] = flat_z;
    bo->vertices[i * vertices_per_quad + 24] = qs[i].r;
    bo->vertices[i * vertices_per_quad + 25] = qs[i].g;
    bo->vertices[i * vertices_per_quad + 26] = qs[i].b;
    bo->vertices[i * vertices_per_quad + 27] = qs[i].a;
  }
}


static uint16_t quad_indices[] = { 0, 1, 2,  0, 2, 3 };
static int quad_indices_size = sizeof(quad_indices);

void quads_write_indices(buffer_object* bo, int count)
{
  for(int i = 0; i < count; ++i) {
    memcpy(bo->indices + i * indices_per_quad, quad_indices, quad_indices_size);
  }
}
