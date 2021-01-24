typedef struct
{
  int touch_ids[3];
  spot_type touch_spot_types[3];
  bool jumped_meantime;

  const int default_steps_till_eval;
  int steps_till_eval;
  int n;

  bool alive;
} logic_data;


void reload_logic(logic_data* logic, death_data* death)
{
  logic->steps_till_eval = logic->default_steps_till_eval;
  logic->n = 0;
  logic->jumped_meantime = false;
  logic->alive = true;

  stop_death(death);

  for (int i = 0; i < logic->steps_till_eval; ++i) {
    logic->touch_ids[i] = -1;
  }
}


void deactivate_spots(const int id, map_data* map)
{
  spot_type type = map->spot_types[id];

  for (int i = 0; i < map->n; ++i) {
    if (map->spot_types[i] == type) {
      map->spot_statuses[i] = spot_inactive;

      map->rects[i].r = dead_type_colors[type].r;
      map->rects[i].g = dead_type_colors[type].g;
      map->rects[i].b = dead_type_colors[type].b;
    }
  }
}


void reset_spots(map_data* map)
{
  for (int i = 0; i < map->n; ++i) {
    map->spot_statuses[i] = spot_active;

    map->rects[i].r = type_colors[map->spot_types[i]].r;
    map->rects[i].g = type_colors[map->spot_types[i]].g;
    map->rects[i].b = type_colors[map->spot_types[i]].b;
  }
}


void matrix_xy(const rect* rect, const map_data* map, int* x, int* y)
{
  *x = ((rect->x1 + rect->x2) * 0.5f + 1.0f) / map->raw_tile_width;
  *y = ((rect->y1 + rect->y2) * 0.5f + 1.0f) / map->raw_tile_height;
}

int matrix_i(const map_data* map, const int x, const int y)
{
  return y * map->matrix_w + x;
}


bool death_on(const map_data* map, death_data* death, const int k)
{
  float x1, y1, x2, y2;

  if (map->matrix[k] != -1) return false;

  if (death->matrix[k] != -1) return true;

  death->matrix[k] = 1;
  raw_xy12(map, k, &x1, &y1, &x2, &y2);
  death->rects[death->n] = (rect){
    x1,
    y1,
    x2,
    y2,
    0.7f,
    0.3f,
    0.3f,
    1.0f
  };
  death->prev_rects[death->n] = death->rects[death->n];

  death->n += 1;

  return true;
}


void evaluate
(player_data* player, const float t, map_data* map, logic_data* logic,
 death_data* death)
{
  int spot_x, spot_y, j;
  float x1, y1, x2, y2;
  for (int i = 0; i < map->n; ++i) {
    if (map->spot_statuses[i] == spot_active) {
      matrix_xy(&map->rects[i], map, &spot_x, &spot_y);

      // death->matrix[matrix_i(map, spot_x, spot_y)] = 1;

      j = spot_x + 1;
      while(j < map->matrix_w && death_on(map, death, matrix_i(map, j, spot_y))) {
        ++j;
      }

      j = spot_x - 1;
      while(j >= 0 && death_on(map, death, matrix_i(map, j, spot_y))) {
        --j;
      }

      j = spot_y + 1;
      while(j < map->matrix_h && death_on(map, death, matrix_i(map, spot_x, j))) {
        ++j;
      }

      j = spot_y - 1;
      while(j >= 0 && death_on(map, death, matrix_i(map, spot_x, j))) {
        --j;
      }
    }
  }

  start_death(death, t);
}


void update
(player_data* player, const float t, const float dt, const input_data* in,
 map_data* map, logic_data* logic, death_data* death)
{
  player->just_jumped = false;

  update_player_positions(player, t, dt, in, map);

  if (player->just_jumped) {
    logic->jumped_meantime = true;
  }

  check_collisions(player, map);


  int logic_x, logic_y;
  matrix_xy(&player->rect, map, &logic_x, &logic_y);

  // sdtx_printf("step %d - %s\n pos %f %f - %d %d", logic->n, logic->alive ? "alive" : "dead", player->rect.x1, player->rect.y1, logic_x, logic_y);
  // sdtx_printf("w h %d %d", sapp_width(), sapp_height());


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

  for (int i = 0; i < map->n; ++i) {
    if (player->rect.x1 < map->rects[i].x1) {
      temp_overlap = player->rect.x1 + player->width - map->rects[i].x1;
    } else if (player->rect.x2 <= map->rects[i].x2) {
      temp_overlap = player->width;
    } else {
      temp_overlap = map->rects[i].x2 - player->rect.x1;
    }

    if (temp_overlap > bottom_overlap &&
        fabs(player->rect.y1 - map->rects[i].y2) < eps) {
      bottom_id = i;
      bottom_spot_type = map->spot_types[i];
      bottom_overlap = temp_overlap;
    }

    if (temp_overlap > top_overlap &&
        fabs(map->rects[i].y1 - player->rect.y2) < eps) {
      top_id = i;
      top_spot_type = map->spot_types[i];
      top_overlap = temp_overlap;
    }


    if (player->rect.y1 < map->rects[i].y1) {
      temp_overlap = player->rect.y1 + player->height - map->rects[i].y1;
    } else if (player->rect.y2 <= map->rects[i].y2) {
      temp_overlap = player->width;
    } else {
      temp_overlap = map->rects[i].y2 - player->rect.y1;
    }

    if (temp_overlap > left_overlap &&
        fabs(player->rect.x1 - map->rects[i].x2) < eps &&
        (player->rect.x1 - player->prev_rect.x1 < 0.0f ||
         in->h == IN_LEFT)) {
      left_id = i;
      left_spot_type = map->spot_types[i];
      left_overlap = temp_overlap;
    }

    if (temp_overlap > right_overlap &&
        fabs(map->rects[i].x1 - player->rect.x2) < eps &&
        (player->rect.x1 - player->prev_rect.x1 > 0.0f ||
         in->h == IN_RIGHT)) {
      right_id = i;
      right_spot_type = map->spot_types[i];
      right_overlap = temp_overlap;
    }
  }

  // sdtx_printf("%d. found_id: %d\n", logic->n, found_id);
  // printf("%d. found_id: %d -- %d << %d\n", logic->n, found_id, (logic->n + 2) % logic->steps_till_eval, logic->jumped_meantime);

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
      (!logic->jumped_meantime &&
       logic->touch_ids[(logic->n + 2) % logic->steps_till_eval] == found_id)) {
    return;
  }

  logic->jumped_meantime = false;

  // printf("----- %d %d %d %d\n", bottom_id, top_id, left_id, right_id);

  if (bottom_id != -1 && top_id == -1 && left_id == -1 && right_id == -1) {
    logic->touch_ids[logic->n] = bottom_id;
    logic->touch_spot_types[logic->n] = bottom_spot_type;
    deactivate_spots(bottom_id, map);
    logic->n += 1;
  } else if (bottom_id == -1 && top_id != -1 && left_id == -1 && right_id == -1) {
    logic->touch_ids[logic->n] = top_id;
    logic->touch_spot_types[logic->n] = top_spot_type;
    deactivate_spots(top_id, map);
    logic->n += 1;
  } else if (bottom_id == -1 && top_id == -1 && left_id != -1 && right_id == -1) {
    logic->touch_ids[logic->n] = left_id;
    logic->touch_spot_types[logic->n] = left_spot_type;
    deactivate_spots(left_id, map);
    logic->n += 1;
  } else if (bottom_id == -1 && top_id == -1 && left_id == -1 && right_id != -1) {
    logic->touch_ids[logic->n] = right_id;
    logic->touch_spot_types[logic->n] = right_spot_type;
    deactivate_spots(right_id, map);
    logic->n += 1;
  }

  if (logic->n == logic->steps_till_eval) {
    evaluate(player, t, map, logic, death);
    logic->n = 0;
    reset_spots(map);
    // printf("reset!\n");
  } else {
    stop_death(death);
  }
}
