typedef struct
{
  int n;
  rect* prev_rects;
  rect* rects;

  int* matrix; // matrix, linear, but 2d
  int matrix_size;

  schedule_data_t schedule;
  tween_data_t tween;

  bool player_dead;
} death_data_t;


death_data_t death_data;

color death_color = {0.9f, 0.5f, 0.7f};


void reset_killing()
{
  death_data.n = 0;
  memset(death_data.matrix, -1, death_data.matrix_size * sizeof(int));
}


const float killing_length = 3.0f;

void show_killing(const float t)
{
  // reset_schedule(&death_data.schedule);
  add_schedule(&death_data.schedule, t + killing_length, reset_killing);

  death_data.tween.start_t = t;
  death_data.tween.end_t = t + killing_length;
  death_data.tween.start_v = 0.0f;
  death_data.tween.end_v = 1.0f;
}


static const float death_tween_subpart = 0.25f;

void show_death(const float t)
{
  death_data.tween.start_t = t;
  death_data.tween.end_t = t + killing_length * death_tween_subpart;
  death_data.tween.start_v = 0.0f;
  death_data.tween.end_v = 1.0f * death_tween_subpart;
}


void reset_death()
{
  reset_killing();

  reset_schedule(&death_data.schedule);

  death_data.player_dead = false;
}


void init_death()
{
  death_data.matrix_size = map_data.matrix_size;

  death_data.prev_rects = (rect*)malloc(death_data.matrix_size * sizeof(rect));
  death_data.rects = (rect*)malloc(death_data.matrix_size * sizeof(rect));

  death_data.matrix = (int*)malloc(death_data.matrix_size * sizeof(int));

  death_data.tween.fn = lerp_tween;

  reset_death();
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
  add_rects(&death_bo, death_data.rects, death_data.prev_rects, death_data.n, frame_fraction);
}
