typedef struct
{
  int x, y;
  rect rect;
  buffer_object bo;
} editor_data;


void init_editor(editor_data* editor)
{
  init_rects_buffer_object(&editor->bo, 1);

  editor->x = editor->y = 0;

  float tw = tile_width / sapp_width();
  float th = tile_height / sapp_height();

  editor->rect = (rect){-1.0f, -1.0f, -1.0f + tw, -1.0f + th, 0.6f, 0.6f, 0.6f, 0.2f};
  move_rect(&editor->rect, editor->x * tw, editor->y * th);

  rects_write_vertices_simple(&editor->rect, &editor->bo, 1);
  rects_write_indices(&editor->bo, 1);
  update_buffer_indices(&editor->bo);
}


void update_editor
(editor_data* editor, const float t, const float dt, const input_data* in, map_data* md)
{
  (void)t; (void)dt; (void) md;

  float tw = tile_width / sapp_width();
  float th = tile_height / sapp_height();

  int px = (- (in->h == IN_LEFT) + (in->h == IN_RIGHT));
  int py = ((in->v == IN_UP) - (in->v == IN_DOWN));

  editor->x += px;
  editor->y += py;
  move_rect(&editor->rect, px * tw, py * th);
}


void draw_editor(editor_data* editor)
{
  rects_write_vertices_simple(&editor->rect, &editor->bo, 1);
  update_buffer_vertices(&editor->bo);

  draw_buffer_object(&editor->bo);
}


void next_spot_type(editor_data* editor, map_data* md)
{
  set_raw_spot(md, editor->x, editor->y, (get_raw_spot(md, editor->x, editor->y) + 1) % spot_type_n);
}


void kill_spot(editor_data* editor, map_data* md)
{
  set_raw_spot(md, editor->x, editor->y, spot_empty);
}
