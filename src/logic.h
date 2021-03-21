int mniam_id;
const int mniam_n = 3;
const char* mniams[mniam_n] = { "mniam.wav", "pyszne.wav", "pyszneczekoladka.wav" };
int kupa_id;
const int kupa_n = 2;
const char* kupas[kupa_n] = { "kupa.wav", "obrzydliwe.wav" };
int sklep_id;
const int sklep_n = 2;
const char* skleps[sklep_n] = { "tuptup.wav", "pik.wav" };
int auto_id;
const int auto_n = 2;
const char* autos[auto_n] = { "yeah.wav", "hurra.wav" };
int wygrana_id;
const int wygrana_n = 2;
const char* wygranas[wygrana_n] = { "wygrana.wav", "swietnie.wav" };


#include "time.h"
#include "stdlib.h"


void reload_logic()
{
  player_data.won = false;
  srand(time(NULL));
}



void draw_logic(const float frame_fraction)
{
  (void)frame_fraction;
}


void add_random_car()
{
  int i, j, ii;
  while(true) {
    i = 3 + rand() % (map_data.matrix_w - 8);
    j = 3 + rand() % (map_data.matrix_h - 9);

    if (map_data.matrix[ij_to_ii(i, j)] == -1) {
      printf("random %d %d\n", i, j);
      set_ij_spot(i, j, spot_car);
      return;
    }
  }
}


int diff_i = 0, diff_j = 0;
int last_diff_i = 0, last_diff_j = 0;
int last_jj = -1;
int last_i = 0, last_j = 0;

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

  if (last_i != player_data.i || last_j != player_data.j) {
    last_jj = -1;
  }

  last_diff_i = diff_i;
  last_diff_j = diff_j;

  last_i = player_data.i;
  last_j = player_data.j;

  int ii = ij_to_ii(player_data.i, player_data.j);
  int jj = map_data.matrix[ii];
  if (jj != -1) {

    if (jj == last_jj) {
      return;
    }

    switch (map_data.spot_types[jj]) {
      case spot_chocolate:
        play_audio(mniams[mniam_id]);
        mniam_id = (mniam_id + 1) % mniam_n;

        last_jj = -1;
        break;

      case spot_shit:
        play_audio(kupas[kupa_id]);
        kupa_id = (kupa_id + 1) % kupa_n;

        last_jj = -1;
        break;

      case spot_shop:
        printf("sklep\n");
        play_audio(skleps[sklep_id]);
        sklep_id = (sklep_id + 1) % sklep_n;

        last_jj = jj;

        add_random_car();
        break;

      case spot_car:
        play_audio(autos[auto_id]);
        auto_id = (auto_id + 1) % auto_n;

        last_jj = -1;
        break;
    }

    if (last_jj == -1) {
      remove_ij_spot(player_data.i, player_data.j);
    }
  }

  int shop_spots_n = 0;
  for (int i = 0; i < map_data.n; ++i) {
    shop_spots_n += map_data.spot_types[i] == spot_shop;
  }

  if (map_data.n - shop_spots_n == 0) {
    player_data.won = true;
  }
}
