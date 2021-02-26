const int TOUCH_N = 255 * 3;

typedef struct
{
  int touch_ids[TOUCH_N];
  spot_type touch_spot_types[TOUCH_N];
  bool jumped_meantime;

  const int default_steps_till_eval;
  int steps_till_eval;
  int n;

  int display_n;
} logic_data;


logic_data logic = (logic_data){
  .default_steps_till_eval = 3
};


static const float tween_time = 0.25f;

void deactivate_spots(const int id, const float t)
{
  spot_type type = map_data.spot_types[id];

  if (map_data.spot_type_statuses[type] != spot_inactive) {
    map_data.prev_spot_type_statuses[type] = map_data.spot_type_statuses[type];
    map_data.spot_type_statuses[type] = spot_inactive;

    map_data.tween_per_type[type].start_t = t;
    map_data.tween_per_type[type].end_t = t + tween_time;
    // map_data.tween_per_type[type].start_v = 1.0f;
    map_data.tween_per_type[type].end_v = 0.0f;
  }
}


// BEGIN reactivate_spots CLOSURE

float closure_t;
spot_type closure_types_to_reactivate[spot_type_n];
int closure_types_to_reactivate_n;

void reactivate_spots()
{
  const float t = closure_t;

  for (int i = 0; i < closure_types_to_reactivate_n; ++i) {
    map_data.tween_per_type[closure_types_to_reactivate[i]].start_t = t;
    map_data.tween_per_type[closure_types_to_reactivate[i]].end_t = t + killing_length * 0.5f;
    // map_data.tween_per_type[closure_types_to_reactivate[i]].start_v = 0.0f;
    map_data.tween_per_type[closure_types_to_reactivate[i]].end_v = 1.0f;
  }
}


void reset_display_n()
{
  logic.display_n = logic.n % logic.steps_till_eval;
}


void reset_spots(const float t)
{
  closure_types_to_reactivate_n = 0;

  for (int i = 0; i < spot_type_n; ++i) {
    if (map_data.spot_type_statuses[i] == spot_inactive) {
      map_data.spot_type_statuses[i] = spot_active;
      closure_types_to_reactivate[closure_types_to_reactivate_n] = i;
      closure_types_to_reactivate_n += 1;
    }
  }

  closure_t = t + tween_time * 2.0f;
  add_schedule(&map_data.reset_schedule, closure_t, reactivate_spots);

  add_schedule(&map_data.reset_schedule, closure_t + killing_length * 0.3f, reset_display_n);
}


void undo_spot(const float t, const spot_type type)
{

  map_data.spot_type_statuses[type] = map_data.prev_spot_type_statuses[type];
  map_data.tween_per_type[type].start_t = t;
  map_data.tween_per_type[type].end_t = t + tween_time;
  map_data.tween_per_type[type].start_v = 0.0f;
  map_data.tween_per_type[type].end_v = 1.0f;
}

// END reactive spots CLOSURE


void undo(const float t)
{
  fprintf(stderr, "undo rects %d, logic n %d\n", player_data.undo_rects_i, logic.n);
  if (player_data.undo_rects_i > 0 && logic.n > 0) {
    logic.n -= 1;
    logic.display_n = logic.n % logic.steps_till_eval;

    player_data.undo_rects_i = (player_data.undo_rects_i - 1) % UNDO_RECTS_N;
    player_data.rect = player_data.undo_rects[player_data.undo_rects_i];
    player_data.prev_rect = player_data.rect;
    undo_spot(t, logic.touch_spot_types[logic.n]);

    if (death_data.player_dead) {
      reset_killing();
      death_data.player_dead = false;
    }
  }
}


void reload_logic()
{
  logic.steps_till_eval = logic.default_steps_till_eval;
  logic.n = 0;
  logic.display_n = 0;
  logic.jumped_meantime = false;

  for (int i = 0; i < logic.steps_till_eval; ++i) {
    logic.touch_ids[i] = -1;
  }

  reset_death();
  reset_player(900.0f, 1500.0f);
}


void matrix_xy(const rect* rect, int* x, int* y)
{
  *x = ((rect->x1 + rect->x2) * 0.5f + 1.0f) / map_data.raw_tile_width;
  *y = ((rect->y1 + rect->y2) * 0.5f + 1.0f) / map_data.raw_tile_height;
}

int matrix_i(const int x, const int y)
{
  return y * map_data.matrix_w + x;
}


bool player_hit(const float x1, const float y1, const float x2, const float y2)
{
  float mid_x = player_data.rect.x1 + player_data.width * 0.5f;
  float mid_y = player_data.rect.y1 + player_data.height * 0.5f;

  return mid_x >= x1 && mid_x < x2 && mid_y >= y1 && mid_y < y2;
}


bool death_on(const int k)
{
  float x1, y1, x2, y2;

  if (map_data.matrix[k] != -1) return false;

  if (death_data.matrix[k] != -1) return true;

  death_data.matrix[k] = 1;
  raw_xy12(k, &x1, &y1, &x2, &y2);
  death_data.rects[death_data.n] = (rect){
    x1,
    y1,
    x2,
    y2,
    death_color.r,
    death_color.g,
    death_color.b,
    0.0f,
    flat_z - 0.6f,
    -1.0f,
    -1.0f
  };
  death_data.prev_rects[death_data.n] = death_data.rects[death_data.n];

  death_data.n += 1;

  if (player_hit(x1, y1, x2, y2)) { // I have the matrix_xy fn as well.
    death_data.player_dead = true;
  }

  return true;
}


void evaluate(const float t)
{
  int spot_x, spot_y, j;
  for (int i = 0; i < map_data.n; ++i) {
    if (map_data.spot_types[i] != spot_neutral &&
        map_data.spot_type_statuses[map_data.spot_types[i]] == spot_active) {
      matrix_xy(&map_data.rects[i], &spot_x, &spot_y);

      // death->matrix[matrix_i(spot_x, spot_y)] = 1;

      j = spot_x + 1;
      while(j < map_data.matrix_w && death_on(matrix_i(j, spot_y))) {
        ++j;
      }

      j = spot_x - 1;
      while(j >= 0 && death_on(matrix_i(j, spot_y))) {
        --j;
      }

      j = spot_y + 1;
      while(j < map_data.matrix_h && death_on(matrix_i(spot_x, j))) {
        ++j;
      }

      j = spot_y - 1;
      while(j >= 0 && death_on(matrix_i(spot_x, j))) {
        --j;
      }
    }
  }


  if (death_data.player_dead) {
    show_death(t);
  } else {
    show_killing(t);
    reset_spots(t);
  }
}


// const float twitch_size = 20.0f;
//
// void twitch_fn()
// {
//   player_data.rect.x1 += twitch_size;
// }
//
//
// void twitch_back_fn()
// {
//   player_data.rect.x1 -= twitch_size;
// }


void draw_logic(const float frame_fraction)
{
  (void)frame_fraction;

  sdtx_origin(15.0f, 10.0f);
  sdtx_font(0);
  sdtx_color4f(0.5f, 0.5f, 0.5f, 0.75f);
  sdtx_printf("%d\n", logic.display_n);
}


void update_logic
(const float t, const float dt)
{
  player_data.just_jumped = false;

  update_player_positions(t, dt);

  if (player_data.just_jumped) {
    logic.jumped_meantime = true;
  }

  check_collisions();


  int logic_x, logic_y;
  matrix_xy(&player_data.rect, &logic_x, &logic_y);



  // if (in_data.v == IN_UP) {
  //   add_schedule(&player_data.twitch_schedule, t + dt * 20.0f, twitch_fn);
  //   add_schedule(&player_data.twitch_schedule, t + dt * 30.0f, twitch_back_fn);
  // }
  // execute_schedule(&player_data.twitch_schedule, t);



  float eps = 0.0011f;

  int bottom_id = -1;
  spot_type bottom_spot_type = -1;
  float bottom_overlap = 0.0f;
  int top_id = -1;
  spot_type top_spot_type = -1;
  float top_overlap = 0.0f;
  int left_id = -1;
  spot_type left_spot_type = -1;
  float left_overlap = 0.0f;
  int right_id = -1;
  spot_type right_spot_type = -1;
  float right_overlap = 0.0f;

  float temp_overlap;
  int found_id = -1;

  for (int i = 0; i < map_data.n; ++i) {
    if (player_data.rect.x1 < map_data.rects[i].x1) {
      temp_overlap = player_data.rect.x1 + player_data.width - map_data.rects[i].x1;
    } else if (player_data.rect.x2 <= map_data.rects[i].x2) {
      temp_overlap = player_data.width;
    } else {
      temp_overlap = map_data.rects[i].x2 - player_data.rect.x1;
    }

    if (temp_overlap > bottom_overlap &&
        fabs(player_data.rect.y1 - map_data.rects[i].y2) < eps) {
      bottom_id = i;
      bottom_spot_type = map_data.spot_types[i];
      bottom_overlap = temp_overlap;
    }

    if (temp_overlap > top_overlap &&
        fabs(map_data.rects[i].y1 - player_data.rect.y2) < eps) {
      top_id = i;
      top_spot_type = map_data.spot_types[i];
      top_overlap = temp_overlap;
    }


    if (player_data.rect.y1 < map_data.rects[i].y1) {
      temp_overlap = player_data.rect.y1 + player_data.height - map_data.rects[i].y1;
    } else if (player_data.rect.y2 <= map_data.rects[i].y2) {
      temp_overlap = player_data.width;
    } else {
      temp_overlap = map_data.rects[i].y2 - player_data.rect.y1;
    }

    if (temp_overlap > left_overlap &&
        fabs(player_data.rect.x1 - map_data.rects[i].x2) < eps &&
        (player_data.rect.x1 - player_data.prev_rect.x1 < 0.0f ||
         in_data.h == IN_LEFT)) {
      left_id = i;
      left_spot_type = map_data.spot_types[i];
      left_overlap = temp_overlap;
    }

    if (temp_overlap > right_overlap &&
        fabs(map_data.rects[i].x1 - player_data.rect.x2) < eps &&
        (player_data.rect.x1 - player_data.prev_rect.x1 > 0.0f ||
         in_data.h == IN_RIGHT)) {
      right_id = i;
      right_spot_type = map_data.spot_types[i];
      right_overlap = temp_overlap;
    }
  }


  if ((bottom_id != -1 && top_id != -1) ||
      (left_id != -1 && right_id != -1)) {
    // death
    exit(0);
  } else if (
      (left_id != -1 && top_id != -1) ||
      (left_id != -1 && bottom_id != -1) ||
      (right_id != -1 && top_id != -1) ||
      (right_id != -1 && bottom_id != -1)) {
    // double touch

    // printf("double touch\n\n");
    return;
  } else {
    found_id = left_id + right_id + top_id + bottom_id + 3;
  }


  if (found_id == -1 ||
      (!logic.jumped_meantime &&
       logic.n > 0 &&
       logic.touch_ids[logic.n - 1] == found_id) ||
      map_data.spot_types[found_id] == spot_neutral) {
    return;
  }

  player_data.undo_rects_i = (player_data.undo_rects_i + 1) % UNDO_RECTS_N;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;

  logic.jumped_meantime = false;

  // printf("----- %d %d %d %d\n", bottom_id, top_id, left_id, right_id);

  if (bottom_id != -1 && top_id == -1 && left_id == -1 && right_id == -1) {
    logic.touch_ids[logic.n] = bottom_id;
    logic.touch_spot_types[logic.n] = bottom_spot_type;
    deactivate_spots(bottom_id, t);
    logic.n += 1;
  } else if (bottom_id == -1 && top_id != -1 && left_id == -1 && right_id == -1) {
    logic.touch_ids[logic.n] = top_id;
    logic.touch_spot_types[logic.n] = top_spot_type;
    deactivate_spots(top_id, t);
    logic.n += 1;
  } else if (bottom_id == -1 && top_id == -1 && left_id != -1 && right_id == -1) {
    logic.touch_ids[logic.n] = left_id;
    logic.touch_spot_types[logic.n] = left_spot_type;
    deactivate_spots(left_id, t);
    logic.n += 1;
  } else if (bottom_id == -1 && top_id == -1 && left_id == -1 && right_id != -1) {
    logic.touch_ids[logic.n] = right_id;
    logic.touch_spot_types[logic.n] = right_spot_type;
    deactivate_spots(right_id, t);
    logic.n += 1;
  }

  logic.display_n = logic.n % logic.steps_till_eval;

  if (logic.n > 0 && logic.n % logic.steps_till_eval == 0) {
    evaluate(t);
  }
}
