typedef struct
{
  int x, y;
  rect r;
  buffer_object bo;
} editor_data;


void init_editor(editor_data* ed)
{
  init_buffer_object(&ed->bo, vertices_per_rect, indices_per_rect);

  ed->x = ed->y = 0;

  float tw = tile_width / sapp_width();
  float th = tile_height / sapp_height();

  ed->r = (rect){-1.0f, -1.0f, -1.0f + tw, -1.0f + th, 0.6f, 0.6f, 0.6f, 0.2f};
  move_rect(&ed->r, ed->x * tw, ed->y * th);

  rects_write_vertices_simple(&ed->r, &ed->bo, 1);
  rects_write_indices(&ed->bo, 1);
  update_buffer_indices(&ed->bo);
}


void update_editor
(editor_data* ed, const float t, const float dt, const input_data* in, map_data* md)
{
  float tw = tile_width / sapp_width();
  float th = tile_height / sapp_height();

  int px = (- (in->h == IN_LEFT) + (in->h == IN_RIGHT));
  int py = ((in->v == IN_UP) - (in->v == IN_DOWN));

  ed->x += px;
  ed->y += py;
  move_rect(&ed->r, px * tw, py * th);
}


void draw_editor(editor_data* ed)
{
  rects_write_vertices_simple(&ed->r, &ed->bo, 1);
  update_buffer_vertices(&ed->bo);

  draw_buffer_object(&ed->bo);
}


void add_spot(editor_data* ed, map_data* md)
{
  set_spot(md, ed->x, ed->y, (spot_type)0);
}


void next_spot_type(editor_data* ed, map_data* md)
{
  set_spot(md, ed->x, ed->y, (get_spot(md, ed->x, ed->y) + 1) % spot_type_n);
}


void kill_spot(editor_data* ed, map_data* md)
{
  set_spot(md, ed->x, ed->y, spot_empty);
}
