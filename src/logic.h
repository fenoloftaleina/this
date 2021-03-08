const int TOUCH_N = 255 * 3;

typedef struct
{
  int prev_found_ids[UNDO_RECTS_N];
} logic_data;


logic_data logic = (logic_data){};


static const float tween_time = 0.25f;

void deactivate_spots(const int id, const float t)
{
  spot_type type = map_data.spot_types[id];

  if (map_data.spot_type_statuses[type] != spot_inactive) {
    map_data.spot_type_statuses[type] = spot_inactive;

    map_data.tween_per_type[type].start_t = t;
    map_data.tween_per_type[type].end_t = t + tween_time;
    // map_data.tween_per_type[type].start_v = 1.0f;
    map_data.tween_per_type[type].end_v = 0.0f;
  }
}



void reload_logic()
{
  player_data.undo_rects_i = 0;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;
  logic.prev_found_ids[player_data.undo_rects_i] = -1;
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
        map_data.spot_types[i] != spot_spikes &&
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
  // sdtx_printf("%d\n", logic.display_n);
}


void undo(const float t)
{
  if (player_data.undo_rects_i > 0) {
    printf("undo!!!!!\n");

    death_data.player_dead = false;

    player_data.undo_rects_i = (player_data.undo_rects_i - 1) % UNDO_RECTS_N;
    player_data.rect = player_data.undo_rects[player_data.undo_rects_i];
    player_data.prev_rect = player_data.rect;
  }
}


void update_logic
(const float t, const float dt)
{
  player_data.just_jumped = false;

  update_player_positions(t, dt);

  check_collisions();


  int logic_x, logic_y;
  matrix_xy(&player_data.rect, &logic_x, &logic_y);

  // printf("??? %d %d\n", logic.n, logic.n_offset);
  // if (get_raw_spot(logic_x, logic_y) == spot_checkpoint &&
  //     (logic.n + logic.n_offset) % logic.steps_till_eval != 0) {
  //   logic.n_offset = - (logic.n % logic.steps_till_eval);
  //   logic.display_n = (logic.n + logic.n_offset) % logic.steps_till_eval;
  //
  //   map_data.tween_per_type[spot_checkpoint].start_t = t;
  //   map_data.tween_per_type[spot_checkpoint].end_t = t + tween_time;
  //   map_data.tween_per_type[spot_checkpoint].start_v = 0.0f;
  //   map_data.tween_per_type[spot_checkpoint].end_v = 1.0f;
  //
  //   for (int i = 0; i < spot_type_n; ++i) {
  //     if (i != spot_checkpoint &&
  //         map_data.spot_type_statuses[i] == spot_inactive) {
  //       map_data.spot_type_statuses[i] = spot_active;
  //
  //       map_data.tween_per_type[i].start_t = t;
  //       map_data.tween_per_type[i].end_t = t + tween_time;
  //       // map_data.tween_per_type[i].start_v = 0.0f;
  //       map_data.tween_per_type[i].end_v = 1.0f;
  //     }
  //   }
  //
  //   return;
  // }


  if (player_data.rect.x1 + player_data.width * 0.5f < 0.0f) {
    player_data.lost = true;
    return;
  }

  if (player_data.rect.y1 < 0.0f) {
    player_data.won = true;
    return;
  }


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

  // printf("found id %d\n", found_id);

  if (found_id == -1 || found_id == logic.prev_found_ids[player_data.undo_rects_i]) {
    return;
  }

  player_data.undo_rects_i = (player_data.undo_rects_i + 1) % UNDO_RECTS_N;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;
  logic.prev_found_ids[player_data.undo_rects_i] = found_id;


  spot_type found_type = map_data.spot_types[found_id];

  if (found_type == spot_spikes) {
    death_data.player_dead = true;
  }
}
