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


void reload_logic(logic_data* logic)
{
  logic->steps_till_eval = logic->default_steps_till_eval;
  logic->n = 0;
  logic->jumped_meantime = false;
  logic->alive = true;
  for (int i = 0; i < logic->steps_till_eval; ++i) {
    logic->touch_ids[i] = -1;
  }
}


void kill_spots(const int id, map_data* map)
{
  spot_type type = map->spot_types[id];

  for (int i = 0; i < map->n; ++i) {
    if (map->spot_types[i] == type) {
      map->spot_statuses[i] = spot_dead;

      map->rects[i].r = dead_type_colors[type].r;
      map->rects[i].g = dead_type_colors[type].g;
      map->rects[i].b = dead_type_colors[type].b;
    }
  }
}


void reset_spots(map_data* map)
{
  for (int i = 0; i < map->n; ++i) {
    map->spot_statuses[i] = spot_alive;

    map->rects[i].r = type_colors[map->spot_types[i]].r;
    map->rects[i].g = type_colors[map->spot_types[i]].g;
    map->rects[i].b = type_colors[map->spot_types[i]].b;
  }
}


void evaluate(player_data* player, map_data* map, logic_data* logic)
{

}


void update
(player_data* player, const float t, const float dt, const input_data* in, map_data* map, logic_data* logic)
{
  player->just_jumped = false;

  update_player_positions(player, t, dt, in, map);

  if (player->just_jumped) {
    logic->jumped_meantime = true;
  }

  check_collisions(player, map);


  sdtx_printf("step %d - %s", logic->n, logic->alive ? "alive" : "dead");


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
      temp_overlap = player->rect.x1 + player->w - map->rects[i].x1;
    } else if (player->rect.x2 <= map->rects[i].x2) {
      temp_overlap = player->w;
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
      temp_overlap = player->rect.y1 + player->h - map->rects[i].y1;
    } else if (player->rect.y2 <= map->rects[i].y2) {
      temp_overlap = player->w;
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

    printf("double touch\n\n");
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
    kill_spots(bottom_id, map);
    logic->n += 1;
  } else if (bottom_id == -1 && top_id != -1 && left_id == -1 && right_id == -1) {
    logic->touch_ids[logic->n] = top_id;
    logic->touch_spot_types[logic->n] = top_spot_type;
    kill_spots(top_id, map);
    logic->n += 1;
  } else if (bottom_id == -1 && top_id == -1 && left_id != -1 && right_id == -1) {
    logic->touch_ids[logic->n] = left_id;
    logic->touch_spot_types[logic->n] = left_spot_type;
    kill_spots(left_id, map);
    logic->n += 1;
  } else if (bottom_id == -1 && top_id == -1 && left_id == -1 && right_id != -1) {
    logic->touch_ids[logic->n] = right_id;
    logic->touch_spot_types[logic->n] = right_spot_type;
    kill_spots(right_id, map);
    logic->n += 1;
  }

  if (logic->n == logic->steps_till_eval) {
    evaluate(player, map, logic);
    logic->n = 0;
    reset_spots(map);
    // printf("reset!\n");
  }
}
