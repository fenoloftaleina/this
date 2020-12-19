typedef struct
{
  buffer_object bo;

  int n;
  rect* prev_rects;
  rect* rects;
  // lerp_state* lerps;

  int* matrix; // matrix, linear, but 2d
  int matrix_size;

  bool display;
} death_data;


void start_death(death_data* death, const float t)
{
  death->display = true;
}


void stop_death(death_data* death)
{
  death->display = false;
  death->n = 0;
  memset(death->matrix, -1, death->matrix_size * sizeof(int));
}


void init_death(death_data* death, const map_data* map)
{
  death->matrix_size = map->matrix_size;

  death->prev_rects = (rect*)malloc(death->matrix_size * sizeof(rect));
  death->rects = (rect*)malloc(death->matrix_size * sizeof(rect));

  death->matrix = (int*)malloc(death->matrix_size * sizeof(int));

  stop_death(death);

  init_rects(&death->bo, map->matrix_size);
}


void draw_death(death_data* death, const float frame_fraction)
{
  if (!death->display) return;

  draw_rects(&death->bo, death->prev_rects, death->rects, death->n, frame_fraction);
}
