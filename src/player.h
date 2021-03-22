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

  int i, j;

  bool won;
  bool lost;
} player_data_t;

player_data_t player_data;


#include "physics.h"


void move_player_to(const int i, const int j)
{
  float x1, y1, x2, y2;
  ij_to_xy(i, j, &x1, &y1, &x2, &y2);

  player_data.i = i;
  player_data.j = j;

  player_data.rect = (rect){x1, y1, x2, y2, 1.0f, 1.0f, 1.0f, 1.0f, flat_z};
}


void reset_player(float start_x, float start_y)
{
  player_data.just_jumped = false;
  player_data.won = false;
  player_data.lost = false;

  init_animation(&player_data.animation, 10, 2);

  move_player_to(1, 5);

  player_data.prev_rect = player_data.rect;

  player_data.undo_rects_i = 0;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;
}


void init_player()
{
  player_data.width = 200.0f;
  player_data.height = 200.0f;

  reset_player(0, 0);
}


void update_player(const float t)
{
  update_animation(&player_data.animation, t);
  set_sprite(&player_data.rect, &texture, player_data.animation.cur_sprite_id);

  player_data.prev_rect = player_data.rect;
}


static const float inset = 15.0f;
static const float thickness = 4.0f;

void draw_player(const float frame_fraction)
{
  add_rects(&sprites_bo, &player_data.rect, &player_data.prev_rect, 1, frame_fraction);
}
