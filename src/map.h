typedef struct
{
  buffer_object bo;
  rect* rs;
  int n;
  int* ts;
} map_data;

typedef struct
{
  float r, g, b;
} color;

color type_colors[] = {
  {0.5, 0.35, 0.47},
  {0.4, 0.6, 0.4},
  {0.3, 0.6, 0.7}
};


static float tile_width = 200.0f;
static float tile_height = 200.0f;

static int max_n = 1000;


void init_map(map_data* md)
{
  init_buffer_object(&md->bo, max_n * vertices_per_rect, max_n * indices_per_rect);
  set_buffer_counts(&md->bo, 0, 0);
}


void draw_map(const map_data* md)
{
  draw_buffer_object(&md->bo);
}


void init_level_physics(map_data* md)
{
  md->n = 3;
  md->rs = (rect*)malloc(md->n * sizeof(rect));
  md->ts = (int*)malloc(md->n * sizeof(int));

  float y = -500.0f / sapp_height();

  md->rs[0] = {
    -0.7f, y, 0.7f, y + 0.1f,
    0.8f, 0.8f, 0.8f, 1.0f
  };

  md->rs[1] = {
    -0.7f, y + 0.4f, -0.3f, y + 0.5f,
    0.8f, 0.8f, 0.8f, 1.0f
  };

  md->rs[2] = {
    0.5f, y + 0.1f, 0.6f, y + 0.2f,
    0.8f, 0.8f, 0.8f, 1.0f
  };

  set_buffer_counts(
      &md->bo,
      rects_write_vertices(md->rs, &md->bo, md->n),
      rects_write_indices(&md->bo, md->n)
      );
  update_buffer_vertices(&md->bo);
  update_buffer_indices(&md->bo);
}


void init_level0(map_data* md)
{
  md->n = 10;
  md->rs = (rect*)malloc(md->n * sizeof(rect));
  md->ts = (int*)malloc(md->n * sizeof(int));

  float tw = tile_width / sapp_width();
  float th = tile_height / sapp_height();

  float y = -500.0f / sapp_height();

  int cur_type;
  float cur_y;
  for(int i = 0; i < md->n; ++i) {
    cur_y = i > md->n / 2 ? y : y + (5 - i) * th;
    cur_type = i % 3;
    md->rs[i] = {
      i * tw - 0.5f,
      cur_y,
      (i + 1) * tw - 0.5f,
      cur_y + th,
      type_colors[cur_type].r,
      type_colors[cur_type].g,
      type_colors[cur_type].b,
      1.0f
    };
    md->ts[i] = cur_type;
  }

  set_buffer_counts(
      &md->bo,
      rects_write_vertices(md->rs, &md->bo, md->n),
      rects_write_indices(&md->bo, md->n)
      );
  update_buffer_vertices(&md->bo);
  update_buffer_indices(&md->bo);
}
