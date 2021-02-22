typedef struct
{
  rect rect;
  rect prev_rect;
  float width;
  float height;

  bool just_jumped;

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

  player_data.rect = (rect){-pw2, -ph2, pw2, ph2, 0.9f, 0.9f, 0.9f, 1.0f, flat_z - 0.5f, -1.0f, -1.0f, -1.0f, -1.0f};
  move_rect(&player_data.rect, 900.0f, 1500.0f);
  player_data.prev_rect = player_data.rect;

  player_data.just_jumped = false;

  // player_data.animation = player_animations_data.animations[RUN_RIGHT];
}


void update_player(const float t)
{
  // update_animation(&player_data.animation, t);
  // set_sprite(&player_data.rect, &texture, player_data.animation.cur_sprite_id);
}

static rect rects[20];
static rect prev_rects[20];
static int rects_n = 0;
static const float leg_height = 40.0f;
static const float leg_side = 20.0f;
static const float leg_apart = 20.0f;
static const float leg_width = (100.0f - leg_side * 2.0f - leg_apart) * 0.5f;

void draw_player(const float frame_fraction)
{
  // add_rects(&rects_bo, &player_data.rect, &player_data.prev_rect, 1, frame_fraction);

  player_data.prev_rect = player_data.rect;


  float body_x1 = player_data.rect.x1;
  float body_y1 = player_data.rect.y1 + leg_height;
  float body_x2 = player_data.rect.x2;
  float body_y2 = player_data.rect.y2;

  float left_leg_x1 = player_data.rect.x1 + leg_side;
  float left_leg_y1 = player_data.rect.y1;
  float left_leg_x2 = player_data.rect.x1 + leg_side + leg_width;
  float left_leg_y2 = player_data.rect.y1 + leg_height;

  float right_leg_x1 = player_data.rect.x1 + leg_side + leg_width + leg_apart;
  float right_leg_y1 = player_data.rect.y1;
  float right_leg_x2 = player_data.rect.x1 + leg_side + leg_width + leg_apart + leg_width;
  float right_leg_y2 = player_data.rect.y1 + leg_height;


  rects_n = 3;
  rects[0] = (rect){
    body_x1,
    body_y1,
    body_x2,
    body_y2,
    player_data.rect.r,
    player_data.rect.g,
    player_data.rect.b,
    player_data.rect.a,
    player_data.rect.z,
    player_data.rect.u1,
    player_data.rect.v1,
    player_data.rect.u2,
    player_data.rect.v2,
  };
  rects[1] = (rect){
    left_leg_x1,
    left_leg_y1,
    left_leg_x2,
    left_leg_y2,
    player_data.rect.r,
    player_data.rect.g,
    player_data.rect.b,
    player_data.rect.a,
    player_data.rect.z,
    player_data.rect.u1,
    player_data.rect.v1,
    player_data.rect.u2,
    player_data.rect.v2,
  };
  rects[2] = (rect){
    right_leg_x1,
    right_leg_y1,
    right_leg_x2,
    right_leg_y2,
    player_data.rect.r,
    player_data.rect.g,
    player_data.rect.b,
    player_data.rect.a,
    player_data.rect.z,
    player_data.rect.u1,
    player_data.rect.v1,
    player_data.rect.u2,
    player_data.rect.v2,
  };

  add_rects(&rects_bo, rects, prev_rects, rects_n, frame_fraction);


  lines_data.thickness = 4;
  float half_thickness = 2.0f;
  body_x1 += half_thickness;
  body_x2 -= half_thickness;
  body_y2 -= half_thickness;
  left_leg_y1 += half_thickness;
  right_leg_y1 += half_thickness;
  pos_t positions[] = {
    {left_leg_x1, left_leg_y1},
    {left_leg_x1, left_leg_y2},
    {body_x1, body_y1},
    {body_x1, body_y2},
    {body_x2, body_y2},
    {body_x2, body_y1},
    {right_leg_x2, right_leg_y2},
    {right_leg_x2, right_leg_y1},
    {right_leg_x1, right_leg_y1},
    {right_leg_x1, right_leg_y2},
    {left_leg_x2, left_leg_y2},
    {left_leg_x2, left_leg_y1},
  };
  add_lines(&lines_bo, positions, positions, sizeof(positions) / sizeof(pos_t), &(col_t){0.1f, 0.1f, 0.1f, 1.0f}, true, frame_fraction);

}
