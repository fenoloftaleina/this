typedef struct
{
  buffer_object bo;

  int n;
  rect* prev_rects;
  rect* rects;
  // lerp_state* lerps;

  bool display;
} death_data;


void start_death(death_data* death, const float t)
{
  death->display = true;

  death->rects[0] = (rect) {
    0.5f,
    0.5f,
    0.7f,
    0.7f,
    0.7f,
    0.3f,
    0.3f,
    1.0f
  };
  death->prev_rects[0] = death->rects[0];
}


void stop_death(death_data* death)
{
  death->display = false;
}


void init_death(death_data* death, const map_data* map)
{
  death->prev_rects = (rect*)malloc(map->matrix_size * sizeof(rect));
  death->rects = (rect*)malloc(map->matrix_size * sizeof(rect));

  init_rects(&death->bo, map->matrix_size);

  start_death(death, 0.0f);
}


void draw_death(death_data* death, const float frame_fraction)
{
  if (!death->display) return;

  draw_rects(&death->bo, death->prev_rects, death->rects, 1, frame_fraction);
}
