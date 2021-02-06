typedef struct
{
  int n;
  rect* prev_rects;
  rect* rects;
  // lerp_state* lerps;

  int* matrix; // matrix, linear, but 2d
  int matrix_size;

  bool display;
} death_data;


death_data death;


void start_death(const float t)
{
  (void)t;

  death.display = true;
}


void stop_death()
{
  death.display = false;
  death.n = 0;
  memset(death.matrix, -1, death.matrix_size * sizeof(int));
}


void init_death()
{
  death.matrix_size = map.matrix_size;

  death.prev_rects = (rect*)malloc(death.matrix_size * sizeof(rect));
  death.rects = (rect*)malloc(death.matrix_size * sizeof(rect));

  death.matrix = (int*)malloc(death.matrix_size * sizeof(int));

  stop_death();
}


void draw_death(const float frame_fraction)
{
  if (!death.display) return;

  add_rects(&rects_bo, death.rects, death.prev_rects, death.n, frame_fraction);
}
