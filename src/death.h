typedef struct
{
  int n;
  rect* prev_rects;
  rect* rects;

  int* matrix; // matrix, linear, but 2d
  int matrix_size;

  schedule_data_t schedule;
  tween_data_t tween;
} death_data_t;


death_data_t CR_STATE death_data;


void stop_death()
{
  death_data.n = 0;
  memset(death_data.matrix, -1, death_data.matrix_size * sizeof(int));
}


void start_death(const float t)
{
  const float death_length = 1.0f;

  add_schedule(&death_data.schedule, t + death_length, stop_death);

  death_data.tween.start_t = t;
  death_data.tween.end_t = t + death_length;
  death_data.tween.start_v = 0.0f;
  death_data.tween.end_v = 0.5f;
}


void init_death()
{
  death_data.matrix_size = map_data.matrix_size;

  death_data.prev_rects = (rect*)malloc(death_data.matrix_size * sizeof(rect));
  death_data.rects = (rect*)malloc(death_data.matrix_size * sizeof(rect));

  death_data.matrix = (int*)malloc(death_data.matrix_size * sizeof(int));

  stop_death();

  reset_schedule(&death_data.schedule);

  death_data.tween.fn = parabola;
}


void update_death(const float t)
{
  update_tween(&death_data.tween, t);

  execute_schedule(&death_data.schedule, t);

  for (int i = 0; i < death_data.n; ++i) {
    death_data.rects[i].a = death_data.tween.v;
  }
}


void draw_death(const float frame_fraction)
{
  add_rects(&rects_bo, death_data.rects, death_data.prev_rects, death_data.n, frame_fraction);
}
