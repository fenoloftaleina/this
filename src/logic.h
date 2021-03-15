const int TOUCH_N = 255 * 3;

typedef struct
{
  bool jumped_meantime;

  int prev_i;
  int prev_j;
} logic_data;


logic_data logic = (logic_data){};


void reload_logic()
{
  logic.jumped_meantime = false;

  player_data.undo_rects_i = 0;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;

  logic.prev_i = -1;
  logic.prev_j = -1;
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


const int TEMP_NON_EMPTY = -2;


void update_matrix()
{
  bool updated_something = false;
  int ii, jj;
  int spots_around_count;

  for (int j = 1; j < map_data.m_h - 1; ++j) {
    for (int i = 1; i < map_data.m_w - 1; ++i) {
      ii = ij_to_ii(i, j);
      jj = map_data.matrix[ii];

      if (jj == TEMP_NON_EMPTY) {
        continue;
      }

      spots_around_count = 0;
      if (map_data.matrix[ii - 1] != -1) {
        spots_around_count += 1;
      }
      if (map_data.matrix[ii + 1] != -1) {
        spots_around_count += 1;
      }
      if (map_data.matrix[ii - map_data.m_w] != -1) {
        spots_around_count += 1;
      }
      if (map_data.matrix[ii + map_data.m_w] != -1) {
        spots_around_count += 1;
      }

      if (jj == -1 && spots_around_count > 2) {
        set_ij_spot(i, j, spot_pushable);
        updated_something = true;
      } else if (jj != -1 && spots_around_count > 2) {
        remove_ij_spot(i, j);
        updated_something = true;
      }
    }
  }

  if (updated_something) {
    // update_matrix();
  }
}


void update_logic
(const float t, const float dt)
{
  update_player_positions(t, dt);

  check_collisions();

  int i, j;

  rect_to_ij(&player_data.rect, &i, &j);

  if (i == logic.prev_i && j == logic.prev_j) {
    return;
  }

  logic.prev_i = i;
  logic.prev_j = j;

  int ii = ij_to_ii(i, j);

  map_data.matrix[ii] = TEMP_NON_EMPTY;

  update_matrix();

  map_data.matrix[ii] = -1;
}
