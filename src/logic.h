const int TOUCH_N = 255 * 3;

typedef struct
{
  bool jumped_meantime;

  int prev_i;
  int prev_j;

  int prev_found_id;
} logic_data;


logic_data logic = (logic_data){};


void reload_logic()
{
  logic.jumped_meantime = false;

  player_data.undo_rects_i = 0;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;

  logic.prev_i = -1;
  logic.prev_j = -1;
  logic.prev_found_id = -2;
}


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
    death_data.player_dead = false;

    player_data.undo_rects_i = (player_data.undo_rects_i - 1) % UNDO_RECTS_N;
    player_data.rect = player_data.undo_rects[player_data.undo_rects_i];
    player_data.prev_rect = player_data.rect;
  }
}


void update_logic
(const float t, const float dt)
{
  update_player_positions(t, dt);

  check_collisions();

  int i, j;

  rect_to_ij(&player_data.rect, &i, &j);

  if (i >= 9) {
    reset_player(map_data.player_start_x, map_data.player_start_y);
  }

  // if (i == logic.prev_i && j == logic.prev_j) {
  //   return;
  // }

  // logic.prev_i = i;
  // logic.prev_j = j;

  int ii = ij_to_ii(i, j);
  int jj = map_data.matrix[ii];

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

  if (found_id == -1 || found_id == logic.prev_found_id) {
    return;
  }

  logic.prev_found_id = found_id;

  player_data.undo_rects_i = (player_data.undo_rects_i + 1) % UNDO_RECTS_N;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;

  int fi, fj;
  rect_to_ij(&map_data.rects[found_id], &fi, &fj);

  bool player_above = player_data.rect.y1 > map_data.rects[found_id].y2;
  // printf("%d %f %d %d %f %d %d\n", player_above, player_data.rect.y1, i, j, map_data.rects[ij_to_ii(fi, fj)].y2, fi, fj);

  if (map_data.matrix[ij_to_ii(fi, fj + 1)] == -1 &&
      (!player_above || map_data.matrix[ij_to_ii(fi, fj + 2)] == -1)) {
    map_data.matrix[ij_to_ii(fi, fj)] = -1;
    map_data.matrix[ij_to_ii(fi, fj + 1)] = found_id;

    if (player_above) {
      player_data.rect.y1 += map_data.raw_tile_height;
      player_data.rect.y2 += map_data.raw_tile_height;
      player_data.prev_rect.y1 += map_data.raw_tile_height;
      player_data.prev_rect.y2 += map_data.raw_tile_height;
    }

    ii_to_jj_rect(ij_to_ii(fi, fj + 1), found_id);
  }
}
