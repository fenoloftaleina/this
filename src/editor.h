typedef struct
{
  int x, y;
  rect rect;
} editor_data;


editor_data editor;


void init_editor()
{
  editor.x = editor.y = 0;

  float tw = tile_width;
  float th = tile_height;

  editor.rect = (rect){-1.0f, -1.0f, -1.0f + tw, -1.0f + th, 0.6f, 0.6f, 0.6f, 0.2f};
  move_rect(&editor.rect, editor.x * tw, editor.y * th);
}


void update_editor
(const float t, const float dt)
{
  (void)t; (void)dt;

  float tw = tile_width;
  float th = tile_height;

  int px = (- (in.h == IN_LEFT) + (in.h == IN_RIGHT));
  int py = ((in.v == IN_UP) - (in.v == IN_DOWN));

  editor.x += px;
  editor.y += py;
  move_rect(&editor.rect, px * tw, py * th);

  sdtx_printf("editor pos %f %f", editor.rect.x1, editor.rect.y1);
}


void draw_editor(const float frame_fraction)
{
  add_rects(&rects_bo, &editor.rect, &editor.rect, 1, frame_fraction);
}


void next_spot_type()
{
  set_raw_spot(editor.x, editor.y, (get_raw_spot(editor.x, editor.y) + 1) % spot_type_n);
}


void clear_spot()
{
  set_raw_spot(editor.x, editor.y, spot_empty);
}
