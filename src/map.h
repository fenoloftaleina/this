typedef struct
{
  buffer_object bo;
  rect* rs;
} map_data;

static float tile_width = 200.0f;
static float tile_height = 200.0f;

static int max_n = 1000;
static int n = 10;


void init_map(map_data* md)
{
  init_buffer_object(&md->bo, max_n * vertices_per_rect, max_n * indices_per_rect);
  set_buffer_counts(&md->bo, 0, 0);
}


void draw_map(const map_data* md)
{
  draw_buffer_object(&md->bo);
}


void init_level0(map_data* md)
{
  md->rs = (rect*)malloc(n * sizeof(rect));

  float tw = tile_width / sapp_width();
  float th = tile_height / sapp_height();

  float y = -500.0f / sapp_height();

  for(int i = 0; i < n; ++i) {
    md->rs[i] = (rect){i * tw - 0.5f, y, (i + 0.9f) * tw - 0.5f, y + th, 0.5f, 0.5f, i / (2.0f * n) + 0.7f, 1.0f};
  }

  set_buffer_counts(
      &md->bo,
      rects_write_vertices(md->rs, &md->bo, n),
      rects_write_indices(&md->bo, n)
      );
  update_buffer_vertices(&md->bo);
  update_buffer_indices(&md->bo);
}
