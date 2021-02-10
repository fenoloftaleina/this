typedef struct
{
  int x, y;
  rect rect;
} editor_data_t;


editor_data_t editor_data;


void init_editor()
{
  editor_data.x = editor_data.y = 0;

  float tw = tile_width;
  float th = tile_height;

  editor_data.rect = (rect){-1.0f, -1.0f, -1.0f + tw, -1.0f + th, 0.6f, 0.6f, 0.6f, 0.2f, flat_z, -1.0f, -1.0f, -1.0f, -1.0f};
  move_rect(&editor_data.rect, editor_data.x * tw, editor_data.y * th);
}


void update_editor
(const float t, const float dt)
{
  (void)t; (void)dt;

  float tw = tile_width;
  float th = tile_height;

  int px = (- (in_data.h == IN_LEFT) + (in_data.h == IN_RIGHT));
  int py = ((in_data.v == IN_UP) - (in_data.v == IN_DOWN));

  editor_data.x += px;
  editor_data.y += py;
  move_rect(&editor_data.rect, px * tw, py * th);

  sdtx_printf("editor pos %f %f", editor_data.rect.x1, editor_data.rect.y1);
}


void draw_editor(const float frame_fraction)
{
  add_rects(&rects_bo, &editor_data.rect, &editor_data.rect, 1, frame_fraction);
}


void next_spot_type()
{
  set_raw_spot(editor_data.x, editor_data.y, (get_raw_spot(editor_data.x, editor_data.y) + 1) % spot_type_n);
}


void clear_spot()
{
  set_raw_spot(editor_data.x, editor_data.y, spot_empty);
}
