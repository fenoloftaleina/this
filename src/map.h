typedef enum spot_status
{
  spot_alive,
  spot_dead
} spot_status;

typedef enum spot_type
{
  spot_empty = -1,
  spot_red,
  spot_green,
  spot_blue
} spot_type;


typedef struct map_data
{
  buffer_object bo;

  int n;
  rect* prs; // prev
  rect* rs;
  spot_type* ts;
  spot_status* ss;

  int* m; // matrix, linear, but 2d

  bool changed;

  // int offset_x, offsey_y; // for moving between maps potentially
} map_data;


typedef struct color
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

static const int matrix_w = 20;
static const int matrix_h = 12;
static const int matrix_size = matrix_w * matrix_h;


const int spot(const map_data* md, const int x, const int y)
{
  return md->m[y * matrix_w + x];
}


void init_map(map_data* md)
{
  init_buffer_object(&md->bo, matrix_size * vertices_per_rect, matrix_size * indices_per_rect);
  set_buffer_counts(&md->bo, 0, 0);

  md->rs = (rect*)malloc(matrix_size * sizeof(rect));
  md->ts = (spot_type*)malloc(matrix_size * sizeof(spot_type));
  md->ss = (spot_status*)malloc(matrix_size * sizeof(spot_status));
  md->m = (int*)malloc(matrix_size * sizeof(int));
  memset(md->m, -1, matrix_size * sizeof(int));

  md->changed = false;
}


void draw_map(map_data* md, const float frame_fraction)
{
  if (md->changed) {
    rects_write_vertices(md->prs, md->rs, &md->bo, md->n, frame_fraction),
    update_buffer_vertices(&md->bo);
  }

  draw_buffer_object(&md->bo);
}


void init_level0(map_data* md)
{
  spot_type* spots = (spot_type*)malloc(matrix_size * sizeof(spot_type));
  memset(spots, -1, matrix_size * sizeof(spot_type));
  spots[2] = spot_red;
  spots[45] = spot_green;


  float tw = tile_width / sapp_width();
  float th = tile_height / sapp_height();

  float start_x = -500.0f / sapp_width();
  float start_y = -500.0f / sapp_height();


  spot_type cur_type;
  float cur_x, cur_y;
  int j = 0;
  for(int i = 0; i < matrix_size; ++i) {
    if (spots[i] != -1) {
      cur_type = spots[i];

      cur_x = start_x + (i % matrix_w * tw);
      cur_y = start_y + (i / matrix_w * th);

      md->m[i] = j;
      md->rs[j] = (rect){
        cur_x,
        cur_y,
        cur_x + tw,
        cur_y + th,
        type_colors[cur_type].r,
        type_colors[cur_type].g,
        type_colors[cur_type].b,
        1.0f
      };
      md->ts[j] = cur_type;
      md->ss[j] = spot_alive;

      ++j;
    }
  }

  md->n = j;

  set_buffer_counts(
      &md->bo,
      rects_write_vertices_simple(md->rs, &md->bo, md->n),
      rects_write_indices(&md->bo, md->n)
      );
  update_buffer_vertices(&md->bo);
  update_buffer_indices(&md->bo);

  md->changed = false;
}
