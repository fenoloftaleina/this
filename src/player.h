const int UNDO_RECTS_N = 255;

typedef struct
{
  rect rect;
  rect prev_rect;
  float width;
  float height;

  bool just_jumped;

  animation_data_t animation;

  rect undo_rects[UNDO_RECTS_N];
  int undo_rects_i;

  bool won;
  bool lost;
} player_data_t;

player_data_t player_data;


#include "physics.h"


void reset_player(const float start_x, const float start_y)
{
  player_data.won = false;
  player_data.lost = false;

  player_data.animation = player_animations_data.animations[RUN_RIGHT];

  const float pw2 = player_data.width * 0.5f;
  const float ph2 = player_data.height * 0.5f;

  player_data.rect = (rect){start_x - pw2, start_y - ph2, start_x + pw2, start_y + ph2, 0.9f, 0.9f, 0.9f, 1.0f, flat_z - 0.5f, -1.0f, -1.0f, -1.0f, -1.0f};
  player_data.prev_rect = player_data.rect;

  player_data.undo_rects_i = 0;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;

  set_sprite(&player_data.rect, &texture, player_data.animation.cur_sprite_id);

  reset_death();
}


void init_player()
{
  player_data.width = 100.0f;
  player_data.height = 120.0f;
}


void update_player(const float t)
{
  update_animation(&player_data.animation, t);
  set_sprite(&player_data.rect, &texture, player_data.animation.cur_sprite_id);
}


static const float inset = 15.0f;
static const float thickness = 4.0f;

void draw_player(const float frame_fraction)
{
  if (death_data.player_dead) {
    player_data.rect.g = player_data.rect.b = 0.5f;
  } else {
    player_data.rect.g = player_data.rect.b = 1.0f;
  }

  add_rects(&sprites_bo, &player_data.rect, &player_data.prev_rect, 1, frame_fraction);

  player_data.prev_rect = player_data.rect;
}
