void reload_logic()
{
  player_data.won = false;
}


void draw_logic(const float frame_fraction)
{
  (void)frame_fraction;
}

int diff_i = 0, diff_j = 0;
int last_diff_i = 0, last_diff_j = 0;

void update_logic
(const float t, const float dt)
{
  if (in_data.v == IN_NONE) {
    diff_j = 0;
  }

  if (in_data.h == IN_NONE) {
    diff_i = 0;
  }

  if (in_data.v == IN_UP) {
    diff_j = 1;
  }

  if (in_data.v == IN_DOWN) {
    diff_j = -1;
  }

  if (in_data.h == IN_LEFT) {
    diff_i = -1;
  }

  if (in_data.h == IN_RIGHT) {
    diff_i = 1;
  }

  if (diff_i != last_diff_i) {
    move_player_to(player_data.i + diff_i, player_data.j);
  } else if (diff_j != last_diff_j) {
    move_player_to(player_data.i, player_data.j + diff_j);
  }

  last_diff_i = diff_i;
  last_diff_j = diff_j;

  int ii = ij_to_ii(player_data.i, player_data.j);
  int jj = map_data.matrix[ii];
  if (jj != -1) {
    if (map_data.spot_types[jj] == spot_chocolate) {
      play_audio("mniam.wav");
    } else {
      play_audio("kupa.wav");
    }

    remove_ij_spot(player_data.i, player_data.j);
  }

  if (map_data.n == 0) {
    player_data.won = true;
  }
}
