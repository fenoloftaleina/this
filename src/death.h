typedef struct
{
  int n;
  rect* prev_rects;
  rect* rects;
  // lerp_state* lerps;

  int* matrix; // matrix, linear, but 2d
  int matrix_size;

  bool display;
} death_data_t;


death_data_t death_data;


void start_death(const float t)
{
  (void)t;

  death_data.display = true;
}


void stop_death()
{
  death_data.display = false;
  death_data.n = 0;
  memset(death_data.matrix, -1, death_data.matrix_size * sizeof(int));
}


void init_death()
{
  death_data.matrix_size = map_data.matrix_size;

  death_data.prev_rects = (rect*)malloc(death_data.matrix_size * sizeof(rect));
  death_data.rects = (rect*)malloc(death_data.matrix_size * sizeof(rect));

  death_data.matrix = (int*)malloc(death_data.matrix_size * sizeof(int));

  stop_death();
}


void draw_death(const float frame_fraction)
{
  if (!death_data.display) return;

  add_rects(&rects_bo, death_data.rects, death_data.prev_rects, death_data.n, frame_fraction);
}
