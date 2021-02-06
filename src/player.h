typedef struct
{
  rect rect;
  rect prev_rect;
  float width;
  float height;

  bool just_jumped;
} player_data;

player_data player;


#include "physics.h"


void init_player()
{
  player.width = 100.0f;
  player.height = 120.0f;

  const float pw2 = player.width * 0.5f;
  const float ph2 = player.height * 0.5f;

  player.rect = (rect){-pw2, -ph2, pw2, ph2, 0.3f, 0.3f, 0.3f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
  move_rect(&player.rect, 900.0f, 1500.0f);
  player.prev_rect = player.rect;

  player.just_jumped = false;
}


void draw_player(float frame_fraction)
{
  add_rects(&rects_bo, &player.rect, &player.prev_rect, 1, frame_fraction);
}
