typedef struct
{
  int id;
  int n;
  const char* list[5];
} audio_event_t;

const int audio_events_n = spot_type_n + 2;
audio_event_t audio_events[audio_events_n];

const int wygrana = spot_type_n;
const int koniec = spot_type_n + 1;


void play_audio_event(int event_id)
{
  play_audio(audio_events[event_id].list[audio_events[event_id].id]);
  audio_events[event_id].id = (audio_events[event_id].id + 1) % audio_events[event_id].n;
}


#include "time.h"
#include "stdlib.h"


void reload_logic()
{
  player_data.won = false;
  srand(time(NULL));

  audio_events[spot_chocolate].n = 3;
  audio_events[spot_chocolate].list[0] = "mniam.wav";
  audio_events[spot_chocolate].list[1] = "pyszne.wav";
  audio_events[spot_chocolate].list[2] = "pyszneczekoladka.wav";
  audio_events[spot_shit].n = 2;
  audio_events[spot_shit].list[0] = "kupa.wav";
  audio_events[spot_shit].list[1] = "obrzydliwe.wav";
  audio_events[spot_shop].n = 2;
  audio_events[spot_shop].list[0] = "tuptup.wav";
  audio_events[spot_shop].list[1] = "pik.wav";
  audio_events[spot_bad_shop].n = 3;
  audio_events[spot_bad_shop].list[0] = "zlysklep.wav";
  audio_events[spot_bad_shop].list[1] = "gdzieindziej.wav";
  audio_events[spot_bad_shop].list[2] = "doinnego.wav";
  audio_events[spot_car].n = 2;
  audio_events[spot_car].list[0] = "yeah.wav";
  audio_events[spot_car].list[1] = "hurra.wav";
  audio_events[wygrana].n = 2;
  audio_events[wygrana].list[0] = "wygrana.wav";
  audio_events[wygrana].list[1] = "swietnie.wav";
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
    j = 1 + rand() % (map_data.matrix_h - 3);

    if (map_data.matrix[ij_to_ii(i, j)] == -1) {
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

    if (map_data.spot_types[jj] == spot_shop ||
        map_data.spot_types[jj] == spot_bad_shop) {
      last_jj = jj;
    } else {
      last_jj = -1;
    }

    if (map_data.spot_types[jj] == spot_shop) {
      add_random_car();
    }

    play_audio_event(map_data.spot_types[jj]);

    if (last_jj == -1) {
      remove_ij_spot(player_data.i, player_data.j);
    }
  }

  int any_shop_spots_n = 0;
  for (int i = 0; i < map_data.n; ++i) {
    any_shop_spots_n += (
        map_data.spot_types[i] == spot_shop ||
        map_data.spot_types[i] == spot_bad_shop
        );

  }

  if (map_data.n - any_shop_spots_n == 0) {
    player_data.won = true;
  }
}
