typedef struct
{
  buffer_object bo;
  rect r;
} player_data;

static float player_width = 100.0f;
static float player_height = 120.0f;


void init_player(player_data* pd)
{
  init_buffer_object(&pd->bo, vertices_per_rect, indices_per_rect);

  const float pw2 = player_width / (2.0f * sapp_width());
  const float ph2 = player_height / (2.0f * sapp_height());

  pd->r = (rect){
    -pw2, -ph2, pw2, ph2,
    0.8f, 0.6f, 0.7f
  };

  rects_write_vertices(&pd->r, &pd->bo, 1);
  rects_write_indices(&pd->bo, 1);
  update_buffer_vertices(&pd->bo);
  update_buffer_indices(&pd->bo);
}


void draw_player(const player_data* pd)
{
  draw_buffer_object(&pd->bo);
}
