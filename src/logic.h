const int TOUCH_N = 255 * 3;

typedef struct
{
  bool jumped_meantime;
  int prev_found_id1s[UNDO_RECTS_N];
  int prev_found_id2s[UNDO_RECTS_N];
} logic_data;


logic_data logic = (logic_data){};


void reload_logic()
{
  logic.jumped_meantime = false;

  player_data.undo_rects_i = 0;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;
  logic.prev_found_id1s[player_data.undo_rects_i] = -1;
  logic.prev_found_id2s[player_data.undo_rects_i] = -1;
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


void move_spot
(const int i, const int j, const spot_type type, const int diff_i, const int diff_j)
{
  int ii = ij_to_ii(i, j);
  int jj = map_data.matrix[ii];
  int dest_ii = ij_to_ii(i + diff_i, j + diff_j);
  if (jj == -1 ||
      map_data.matrix[dest_ii] != -1 ||
      map_data.spot_types[jj] != type ||
      i + diff_i == 0 ||
      i + diff_i == map_data.m_w - 1 ||
      j + diff_j == 0 ||
      j + diff_j == map_data.m_h - 1) {
    return;
  }

  map_data.matrix[dest_ii] = jj;
  map_data.matrix[ii] = -1;

  printf("move %d %d by %d %d\n", i, j, diff_i, diff_j);

  ii_to_jj_rect(dest_ii, jj);
}


void run_for(int found_id)
{
  spot_type found_type = map_data.spot_types[found_id];

  if (found_type == spot_spikes) {
    death_data.player_dead = true;

    return;
  }

  int ii;

  int p_i, p_j;
  rect_to_ij(&player_data.rect, &p_i, &p_j);

  int f_i, f_j;
  rect_to_ij(&map_data.rects[found_id], &f_i, &f_j);

  // map_data.matrix[ij_to_ii(i, j)] = -1;

  if (p_i < f_i) {
    // from left
    for (int j = 1; j < map_data.m_h - 1; ++j) {
      for (int i = map_data.m_w - 2; i > 0; --i) {
        move_spot(i, j, found_type, 1, 0);
      }
    }
  } else if (p_i > f_i) {
    // from right
    for (int j = 1; j < map_data.m_h - 1; ++j) {
      for (int i = 1; i < map_data.m_w - 1; ++i) {
        move_spot(i, j, found_type, -1, 0);
      }
    }
  } else if (p_j < f_j) {
    // from bottom
    for (int j = map_data.m_h - 2; j > 0; --j) {
      for (int i = 1; i < map_data.m_w - 1; ++i) {
        move_spot(i, j, found_type, 0, 1);
      }
    }
  } else if (p_j > f_j) {
    // from top
    for (int j = 1; j < map_data.m_h - 1; ++j) {
      for (int i = 1; i < map_data.m_w - 1; ++i) {
        move_spot(i, j, found_type, 0, -1);
      }
    }
  }
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


  if (player_data.rect.x1 + player_data.width * 0.5f < 0.0f) {
    // player_data.lost = true;
    return;
  }

  if (player_data.rect.y1 < 0.0f) {
    // player_data.won = true;
    return;
  }


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
  int found_id1 = -1;
  int found_id2 = -1;

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
  } else if (left_id != -1 && top_id != -1) {
    found_id1 = left_id;
    found_id2 = top_id;
  } else if (left_id != -1 && bottom_id != -1) {
    found_id1 = left_id;
    found_id2 = bottom_id;
  } else if (right_id != -1 && top_id != -1) {
    found_id1 = right_id;
    found_id2 = top_id;
  } else if (right_id != -1 && bottom_id != -1) {
    found_id1 = right_id;
    found_id2 = bottom_id;
  } else {
    found_id1 = left_id + right_id + top_id + bottom_id + 3;
  }

  // printf("found id %d\n", found_id);

  if (found_id1 == -1) {
    return;
  }

  if ((found_id1 == logic.prev_found_id1s[player_data.undo_rects_i] ||
        found_id1 == logic.prev_found_id2s[player_data.undo_rects_i]) &&
       !logic.jumped_meantime) {
    found_id1 = -1;
  }

  if ((found_id2 == logic.prev_found_id1s[player_data.undo_rects_i] ||
        found_id2 == logic.prev_found_id2s[player_data.undo_rects_i]) &&
       !logic.jumped_meantime) {
    found_id2 = -1;
  }

  if (found_id1 == -1 && found_id2 == -1) {
    return;
  }

  logic.jumped_meantime = false;

  player_data.undo_rects_i = (player_data.undo_rects_i + 1) % UNDO_RECTS_N;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;
  if (found_id1 != -1) {
    logic.prev_found_id1s[player_data.undo_rects_i] = found_id1;
    run_for(found_id1);
  }
  if (found_id2 != -1) {
    logic.prev_found_id2s[player_data.undo_rects_i] = found_id2;
    run_for(found_id2);
  }
}
