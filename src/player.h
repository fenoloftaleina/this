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
} player_data_t;

player_data_t player_data;


#include "physics.h"


void reset_player(const float start_x, const float start_y)
{
  player_data.just_jumped = false;

  // player_data.animation = player_animations_data.animations[RUN_RIGHT];

  const float pw2 = player_data.width * 0.5f;
  const float ph2 = player_data.height * 0.5f;

  player_data.rect = (rect){start_x - pw2, start_y - ph2, start_x + pw2, start_y + ph2, 0.9f, 0.9f, 0.9f, 1.0f, flat_z - 0.5f, -1.0f, -1.0f, -1.0f, -1.0f};
  player_data.prev_rect = player_data.rect;

  player_data.undo_rects_i = 0;
  player_data.undo_rects[player_data.undo_rects_i] = player_data.rect;
}


void init_player()
{
  player_data.width = 100.0f;
  player_data.height = 120.0f;

  float start_x = 900.0f, start_y = 1500.0f;
  reset_player(start_x, start_y);
}


void update_player(const float t)
{
  // update_animation(&player_data.animation, t);
  // set_sprite(&player_data.rect, &texture, player_data.animation.cur_sprite_id);
}


static const float inset = 15.0f;
static const float thickness = 4.0f;

void draw_player(const float frame_fraction)
{
  // add_rects(&rects_bo, &player_data.rect, &player_data.prev_rect, 1, frame_fraction);

  float player_x = lerp(player_data.prev_rect.x1, player_data.rect.x1, frame_fraction);
  float player_y = lerp(player_data.prev_rect.y1, player_data.rect.y1, frame_fraction);

  lines_data.thickness = (int)thickness;

  add_lines_rect(
      &lines_bo,
      player_x + thickness, player_y + thickness,
      player_x + player_data.width - thickness, player_y + player_data.height - thickness,
      &(col_t){black_f, black_f, black_f, 1.0f},
      flat_z + 0.5f
      );

  float white_f = 0.865f;
  float gray_f = 0.7f;
  float light_gray_f = 0.77f;
  float in_x1 = 60.0f;
  float in_x2 = 15.0f;
  float in_y1 = 60.0f;
  float in_y2 = 15.0f;
  float glow_diff_top = 15.0f;

  col_t red_tint = {0.8f, 0.1f, 0.3f, 1.0f};
  float red_tween = 0.0f;
  if (death_data.player_dead) {
    red_tween = death_data.tween.v * 2.0f;
  }

  rect player_rects[3];
  player_rects[0] = (rect){
    player_x, player_y, player_x + player_data.width, player_y + player_data.height,
    lerp(white_f, red_tint.r, red_tween),
    lerp(white_f, red_tint.g, red_tween),
    lerp(white_f, red_tint.b, red_tween),
    1.0f,
    flat_z - 0.5f, -1.0f, -1.0f
  };
  player_rects[1] = (rect){
    player_x + in_x1, player_y + in_y1, player_x + player_data.width - in_x2, player_y + player_data.height - in_y2 - glow_diff_top,
    lerp(gray_f, red_tint.r, red_tween),
    lerp(gray_f, red_tint.g, red_tween),
    lerp(gray_f, red_tint.b, red_tween),
    1.0f,
    flat_z - 0.3f, -1.0f, -1.0f
  };
  player_rects[2] = (rect){
    player_x + in_x1, player_y + player_data.height - in_y2 - glow_diff_top, player_x + player_data.width - in_x2, player_y + player_data.height - in_y2,
    lerp(light_gray_f, red_tint.r, red_tween),
    lerp(light_gray_f, red_tint.g, red_tween),
    lerp(light_gray_f, red_tint.b, red_tween),
    1.0f,
    flat_z - 0.3f, -1.0f, -1.0f
  };

  add_rects(&rects_bo, player_rects, player_rects, sizeof(player_rects) / sizeof(rect), frame_fraction);


  // add_lines_rect(
  //     &lines_bo,
  //     player_x + in_x1, player_y + in_y1,
  //     player_x + player_data.width - in_x2, player_y + player_data.height - in_y2,
  //     &(col_t){black_f, black_f, black_f, 1.0f},
  //     flat_z
  //     );


  player_data.prev_rect = player_data.rect;
}
