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

  if (found_type == spot_pushable) {
    int i, j;
    rect_to_ij(&map_data.rects[found_id], &i, &j);

    map_data.matrix[ij_to_ii(i, j)] = -1;

    if (p_i < i) {
      i += 1;
    } else if (p_i > i) {
      i -= 1;
    } else if (p_j < j) {
      j += 1;
    } else if (p_j > j) {
      j -= 1;
    }

    ii = ij_to_ii(i, j);

    if (map_data.matrix[ii] != -1) {
      return;
    }

    map_data.matrix[ii] = found_id;

    ii_to_jj_rect(ii, found_id);
  }
}


void run_interactions()
{
  // clear states
  memset(map_data.states_matrix, state_empty, map_data.m * sizeof(matrix_state));

  for (int j = 1; j < map_data.m_h - 1; ++j) {
    for (int i = 1; i < map_data.m_w - 1; ++i) {
      int ii = ij_to_ii(i, j);
      int jj = map_data.matrix[ii];

      if (jj == -1) {
        continue;
      }

      spot_type type = map_data.spot_types[jj];
      matrix_state state = map_data.states_matrix[jj];

      int left_id = map_data.matrix[ij_to_ii(i - 1, j)];
      int right_id = map_data.matrix[ij_to_ii(i + 1, j)];
      int bottom_id = map_data.matrix[ij_to_ii(i, j - 1)];
      int top_id = map_data.matrix[ij_to_ii(i, j + 1)];

      switch (type) {
        case spot_neutral:
        case spot_empty:
          break;

        case spot_spikes:
          // TODO: everything around needs to die
          break;

        case spot_pushable:
          if (left_id != -1) {
            spot_type left_type = map_data.spot_types[left_id];
            matrix_state left_state = map_data.states_matrix[left_id];

            switch(left_type) {
              case spot_spikes:
                map_data.states_matrix[ii] = state_will_die;
                break;

              case spot_pushable:
                bool can_be_pushed = (right_id == -1);

                if (can_be_pushed) {
                  map_data.states_matrix[ii] = state_will_be_pushed_right;
                }

                switch (left_state) {
                  case state_will_die:
                    break;

                    // conflicting ideas about where to go => impasse
                  case state_will_be_pushed_right:
                  case state_will_be_pushed_down:
                  case state_will_be_pushed_up:
                  case state_push_impasse:
                    map_data.states_matrix[left_id] = state_push_impasse;
                    break;

                  case state_neutral:
                  case state_will_be_pushed_left:
                    map_data.states_matrix[left_id] = state_will_be_pushed_left;
                    break;

                  case state_empty:
                    // just to silence the switch
                    break;
                }
                break;

              case spot_neutral:
                break;
            }
          }
          if (right_id != -1) {
            spot_type right_type = map_data.spot_types[right_id];
            matrix_state right_state = map_data.states_matrix[right_id];

            switch(right_type) {
              case spot_spikes:
                map_data.states_matrix[ii] = state_will_die;
                break;

              case spot_pushable:
                bool can_be_pushed = (left_id == -1);

                if (can_be_pushed) {
                  map_data.states_matrix[ii] = state_will_be_pushed_left;
                }

                switch (right_state) {
                  case state_will_die:
                    break;

                    // conflicting ideas about where to go => impasse
                  case state_will_be_pushed_left:
                  case state_will_be_pushed_down:
                  case state_will_be_pushed_up:
                  case state_push_impasse:
                    map_data.states_matrix[right_id] = state_push_impasse;
                    break;

                  case state_neutral:
                  case state_will_be_pushed_right:
                    map_data.states_matrix[right_id] = state_will_be_pushed_right;
                    break;

                  case state_empty:
                    // just to silence the switch
                    break;
                }
                break;

              case spot_neutral:
                break;
            }
          }
          if (bottom_id != -1) {
            spot_type bottom_type = map_data.spot_types[bottom_id];

            switch(bottom_type) {
              case spot_spikes:
                map_data.states_matrix[ii] = state_will_die;
                break;

              case spot_pushable:
                if (map_data.states_matrix[ii] == state_will_be_pushed_right ||
                    map_data.states_matrix[ii] == state_will_be_pushed_left) {
                  map_data.states_matrix[ii] = state_push_impasse;
                } else {
                  map_data.states_matrix[ii] = state_will_be_pushed_up;
                }
                break;

              case spot_neutral:
                break;
            }
          }
          if (top_id != -1) {
            spot_type top_type = map_data.spot_types[top_id];

            switch(top_type) {
              case spot_spikes:
                map_data.states_matrix[ii] = state_will_die;
                break;

              case spot_pushable:
                if (map_data.states_matrix[ii] == state_will_be_pushed_right ||
                    map_data.states_matrix[ii] == state_will_be_pushed_left ||
                    map_data.states_matrix[ii] == state_will_be_pushed_up) {
                  map_data.states_matrix[ii] = state_push_impasse;
                } else {
                  map_data.states_matrix[ii] = state_will_be_pushed_down;
                }
                break;

              case spot_neutral:
                break;

              default:
                break;
            }
          }
          break;
      }

      // backtrack


    }
  }

  for (int j = 0; j < map_data.m_h; ++j) {
    for (int i = 0; i < map_data.m_w; ++i) {
      int ii = ij_to_ii(i, j);
      int jj = map_data.matrix[ii];

      if (jj == -1) {
        continue;
      }

      matrix_state state = map_data.states_matrix[ii];

      switch (state) {
        case state_will_be_pushed_left:
          move_ij_spot_to_ij(i, j, i - 1, j);
          break;

        case state_will_be_pushed_right:
          move_ij_spot_to_ij(i, j, i + 1, j);
          break;

        case state_will_be_pushed_up:
          move_ij_spot_to_ij(i, j, i, j + 1);
          break;

        case state_will_be_pushed_down:
          move_ij_spot_to_ij(i, j, i, j - 1);
          break;

        case state_will_die:
          remove_ij_spot(i, j);
          break;

        default:
          continue;
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

  run_interactions();
}
