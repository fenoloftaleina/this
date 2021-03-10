typedef struct
{
  int i, j;
  rect rect;
} editor_data_t;


editor_data_t editor_data;


void init_editor()
{
  editor_data.i = editor_data.j = 0;

  float tw = tile_width;
  float th = tile_height;

  editor_data.rect = (rect){-1.0f, -1.0f, -1.0f + tw, -1.0f + th, 0.6f, 0.6f, 0.6f, 0.2f, flat_z, -1.0f, -1.0f, -1.0f, -1.0f};
  move_rect(&editor_data.rect, editor_data.i * tw, editor_data.j * th);
}


void update_editor
(const float t, const float dt)
{
  (void)t; (void)dt;

  float tw = tile_width;
  float th = tile_height;

  int pi = (- (in_data.h == IN_LEFT) + (in_data.h == IN_RIGHT));
  int pj = ((in_data.v == IN_UP) - (in_data.v == IN_DOWN));

  editor_data.i += pi;
  editor_data.j += pj;
  move_rect(&editor_data.rect, pi * tw, pj * th);

  sdtx_printf("editor");
}


void draw_editor(const float frame_fraction)
{
  add_rects(&rects_bo, &editor_data.rect, &editor_data.rect, 1, frame_fraction);
}


void next_spot_type()
{
  set_ij_spot(editor_data.i, editor_data.j, (get_ij_spot(editor_data.i, editor_data.j) + 1) % spot_type_n);
}


void clear_spot()
{
  remove_ij_spot(editor_data.i, editor_data.j);
}


void set_player_start_position()
{
  map_data.player_start_x = (int)(((float)editor_data.i + 0.5f) * map_data.raw_tile_width);
  map_data.player_start_y = (int)(((float)editor_data.j + 0.5f) * map_data.raw_tile_height);

  reset_player(map_data.player_start_x, map_data.player_start_y);
}
