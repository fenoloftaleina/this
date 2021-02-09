typedef struct
{
  rect rect;
  rect prev_rect;
  float width;
  float height;

  bool just_jumped;


  // schedule_data_t twitch_schedule;

  animation_data_t animation;
} player_data_t;

player_data_t player_data;


#include "physics.h"


void init_player()
{
  player_data.width = 100.0f;
  player_data.height = 120.0f;

  const float pw2 = player_data.width * 0.5f;
  const float ph2 = player_data.height * 0.5f;

  player_data.rect = (rect){-pw2, -ph2, pw2, ph2, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
  move_rect(&player_data.rect, 900.0f, 1500.0f);
  player_data.prev_rect = player_data.rect;

  player_data.just_jumped = false;


  // reset_schedule(&player_data.twitch_schedule);

  init_animation(&player_data.animation, 0, 2);
}


void draw_player(float frame_fraction)
{
  add_rects(&sprites_bo, &player_data.rect, &player_data.prev_rect, 1, frame_fraction);
}
