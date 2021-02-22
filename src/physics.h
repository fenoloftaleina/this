#include "math.h"


typedef struct
{
  float gravity;
  float init_v;
  float init_double_v;
  float double_jump_threshold;

  float low_clamp;
  float high_clamp;

  float v;

  bool in_air;
  float started_at;
  bool possible_double_jump;
} jump_data;


typedef struct
{
  float default_clamp;
  float flight_clamp;
  float damping;

  float v;

  float transpose;
  float clamp;
} walk_data;


static float e = 0.1f;


static jump_data jump_state = (jump_data){
  .gravity = -10000.0f,
  .init_v = 1600.0f,
  .init_double_v = 1.655f * 1600.0f,
  .double_jump_threshold = 0.1f,

  .low_clamp = -3.0f * 1600.0f,
  .high_clamp = 2.0f * 1600.0f,

  .v = 0.0f,

  .in_air = true,
  .started_at = 0.0f,
  .possible_double_jump = false
};

static walk_data walk_state = (walk_data){
  .default_clamp = 0.6f * 1200.0f,
  // .default_clamp = 0.1f * 1200.0f, // slow it down for debugging
  .flight_clamp = 0.42f * 1200.0f,
  .damping = 20.0f * 1200.0f,
  .v = 0.0f,
  .transpose = 6.5f * 1200.0f
};


bool lines_intersect
(const float x1, const float y1, const float x2, const float y2,
 const float x3, const float y3, const float x4, const float y4)
{
  float t, u;

  if ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4) == 0) {
    return false;
  } else {
    t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) /
      ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
    u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) /
      ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));

    // x = x1 + t * (x2 - x1);
    // y = y1 + t * (y2 - y1);

    return t >= 0.0 && t <= 1.0 && u >= 0.0 && u <= 1.0;
  }
}


typedef struct
{
  float hanging_x, hanging_y, p_hanging_x, p_hanging_y;
  float dragging_x, dragging_y, p_dragging_x, p_dragging_y;
  float main_x, main_y, p_main_x, p_main_y;
} collision_points_data;

typedef struct
{
  float side_x, side_y1, side_y2;
  float end_x1, end_x2, end_y;
} rect_side_end_data;


void recalc_collision_points
(collision_points_data* collision, const bool up, const bool left)
{
  bool down = !up;
  bool right = !left;

  if (left && down) {
    collision->hanging_x = player_data.rect.x1;
    collision->p_hanging_x = player_data.prev_rect.x1;
    collision->hanging_y = player_data.rect.y2;
    collision->p_hanging_y = player_data.prev_rect.y2;

    collision->dragging_x = player_data.rect.x2;
    collision->p_dragging_x = player_data.prev_rect.x2;
    collision->dragging_y = player_data.rect.y1;
    collision->p_dragging_y = player_data.prev_rect.y1;

    collision->main_x = player_data.rect.x1;
    collision->p_main_x = player_data.prev_rect.x1;
    collision->main_y = player_data.rect.y1;
    collision->p_main_y = player_data.prev_rect.y1;
  } else if (left && up) {
    collision->hanging_x = player_data.rect.x1;
    collision->p_hanging_x = player_data.prev_rect.x1;
    collision->hanging_y = player_data.rect.y1;
    collision->p_hanging_y = player_data.prev_rect.y1;

    collision->dragging_x = player_data.rect.x2;
    collision->p_dragging_x = player_data.prev_rect.x2;
    collision->dragging_y = player_data.rect.y2;
    collision->p_dragging_y = player_data.prev_rect.y2;

    collision->main_x = player_data.rect.x1;
    collision->p_main_x = player_data.prev_rect.x1;
    collision->main_y = player_data.rect.y2;
    collision->p_main_y = player_data.prev_rect.y2;
  } else if (right && down) {
    collision->hanging_x = player_data.rect.x2;
    collision->p_hanging_x = player_data.prev_rect.x2;
    collision->hanging_y = player_data.rect.y2;
    collision->p_hanging_y = player_data.prev_rect.y2;

    collision->dragging_x = player_data.rect.x1;
    collision->p_dragging_x = player_data.prev_rect.x1;
    collision->dragging_y = player_data.rect.y1;
    collision->p_dragging_y = player_data.prev_rect.y1;

    collision->main_x = player_data.rect.x2;
    collision->p_main_x = player_data.prev_rect.x2;
    collision->main_y = player_data.rect.y1;
    collision->p_main_y = player_data.prev_rect.y1;
  } else if (right && up) {
    collision->hanging_x = player_data.rect.x2;
    collision->p_hanging_x = player_data.prev_rect.x2;
    collision->hanging_y = player_data.rect.y1;
    collision->p_hanging_y = player_data.prev_rect.y1;

    collision->dragging_x = player_data.rect.x1;
    collision->p_dragging_x = player_data.prev_rect.x1;
    collision->dragging_y = player_data.rect.y2;
    collision->p_dragging_y = player_data.prev_rect.y2;

    collision->main_x = player_data.rect.x2;
    collision->p_main_x = player_data.prev_rect.x2;
    collision->main_y = player_data.rect.y2;
    collision->p_main_y = player_data.prev_rect.y2;
  }
}


void recalc_rect_side_end
(rect_side_end_data* rect_side_end, const rect* rect, const bool up, const bool left)
{
  bool down = !up;
  bool right = !left;

  rect_side_end->side_y1 = rect->y1;
  rect_side_end->side_y2 = rect->y2;

  rect_side_end->end_x1 = rect->x1;
  rect_side_end->end_x2 = rect->x2;

  if (left && down) {
    rect_side_end->side_x = rect->x2;
    rect_side_end->end_y = rect->y2;
  } else if (left && up) {
    rect_side_end->side_x = rect->x2;
    rect_side_end->end_y = rect->y1;
  } else if (right && down) {
    rect_side_end->side_x = rect->x1;
    rect_side_end->end_y = rect->y2;
  } else if (right && up) {
    rect_side_end->side_x = rect->x1;
    rect_side_end->end_y = rect->y1;
  }
}


void check_collisions()
{
  float eps = 0.001f;

  bool up = jump_state.v > 0;
  bool left = walk_state.v <= 0;
  // bool down = !up;
  // bool right = !left;

  float e_side = left ? eps : -eps;
  float e_end = up ? -eps : eps;

  bool nothing_intersected = true;

  collision_points_data collision;
  recalc_collision_points(&collision, up, left);

  rect_side_end_data rect_side_end;

  for (int i = 0; i < map_data.n; ++i) {
    recalc_rect_side_end(&rect_side_end, &map_data.rects[i], up, left);
    if (lines_intersect(
          collision.p_hanging_x, collision.p_hanging_y, collision.hanging_x, collision.hanging_y,
          rect_side_end.side_x, rect_side_end.side_y1, rect_side_end.side_x, rect_side_end.side_y2)) {
      move_rect(&player_data.rect, rect_side_end.side_x - collision.hanging_x + e_side, 0.0f);
      recalc_collision_points(&collision, up, left);

      nothing_intersected = false;
    }
  }

  for (int i = 0; i < map_data.n; ++i) {
    recalc_rect_side_end(&rect_side_end, &map_data.rects[i], up, left);
    if (lines_intersect(
          collision.p_dragging_x, collision.p_dragging_y, collision.dragging_x, collision.dragging_y,
          rect_side_end.end_x1, rect_side_end.end_y, rect_side_end.end_x2, rect_side_end.end_y)) {
      move_rect(&player_data.rect, 0.0f, rect_side_end.end_y - collision.dragging_y + e_end);
      recalc_collision_points(&collision, up, left);

      jump_state.v = 0.0f;
      jump_state.in_air = up;
      jump_state.started_at = 0.0f;
      jump_state.possible_double_jump = false;

      nothing_intersected = false;
    }
  }

  for (int i = 0; i < map_data.n; ++i) {
    recalc_rect_side_end(&rect_side_end, &map_data.rects[i], up, left);
    if (lines_intersect(
          collision.p_main_x, collision.p_main_y, collision.main_x, collision.main_y,
          rect_side_end.side_x, rect_side_end.side_y1, rect_side_end.side_x, rect_side_end.side_y2)) {
      move_rect(&player_data.rect, rect_side_end.side_x - collision.main_x + e_side, 0.0f);
      recalc_collision_points(&collision, up, left);

      nothing_intersected = false;
    }
  }

  for (int i = 0; i < map_data.n; ++i) {
    recalc_rect_side_end(&rect_side_end, &map_data.rects[i], up, left);
    if (lines_intersect(
          collision.p_main_x, collision.p_main_y, collision.main_x, collision.main_y,
          rect_side_end.end_x1, rect_side_end.end_y, rect_side_end.end_x2, rect_side_end.end_y)) {
      move_rect(&player_data.rect, 0.0f, rect_side_end.end_y - collision.main_y + e_end);
      recalc_collision_points(&collision, up, left);

      jump_state.v = 0.0f;
      jump_state.in_air = up;
      jump_state.started_at = 0.0f;
      jump_state.possible_double_jump = false;

      nothing_intersected = false;
    }
  }

  if (nothing_intersected && !jump_state.in_air) {
    jump_state.v = 0.0f;
    jump_state.in_air = true;
    jump_state.started_at = 0.0f;
    jump_state.possible_double_jump = false;
  }
}


float v_clamp(const float val, const float low_clamp_val, const float high_clamp_val)
{
  return HMM_Clamp(low_clamp_val, val, high_clamp_val);
}

float h_clamp(const float val, const float clamp_val)
{
  return HMM_Clamp(-clamp_val, val, clamp_val);
}


void update_player_positions
(const float t, const float dt)
{
  // printf("%f\n", t);
  if (in_data.v == IN_UP && !jump_state.in_air) {
    jump_state.v = v_clamp(jump_state.init_v, jump_state.low_clamp, jump_state.high_clamp);
    jump_state.in_air = true;
    jump_state.started_at = t;
    jump_state.possible_double_jump = true;

    player_data.just_jumped = true;
  } else if (in_data.v == IN_UP &&
      jump_state.in_air &&
      jump_state.possible_double_jump &&
      t - jump_state.started_at > jump_state.double_jump_threshold
      ) {
    jump_state.v = v_clamp(jump_state.init_double_v, jump_state.low_clamp, jump_state.high_clamp);
    jump_state.possible_double_jump = false;
  } else if (t - jump_state.started_at > jump_state.double_jump_threshold) {
    jump_state.v = v_clamp(jump_state.v + dt * jump_state.gravity, jump_state.low_clamp, jump_state.high_clamp);
  }

  if (in_data.v != IN_UP) {
    jump_state.possible_double_jump = false;
  }

  if (jump_state.in_air) {
    walk_state.clamp = walk_state.flight_clamp;
  } else {
    walk_state.clamp = walk_state.default_clamp;
  }

  if (in_data.h == IN_LEFT) {
    if (player_animations_data.state != RUN_LEFT) {
      player_animations_data.state = RUN_LEFT;
      player_data.animation = player_animations_data.animations[RUN_LEFT];
    }

    walk_state.v = h_clamp(walk_state.v - dt * walk_state.transpose, walk_state.clamp);
  } else if (in_data.h == IN_RIGHT) {
    if (player_animations_data.state != RUN_RIGHT) {
      player_animations_data.state = RUN_RIGHT;
      player_data.animation = player_animations_data.animations[RUN_RIGHT];
    }
    walk_state.v = h_clamp(walk_state.v + dt * walk_state.transpose, walk_state.clamp);
  } else {
    walk_state.v /= (1.0f + walk_state.damping * dt);
    if (fabs(walk_state.v) < e) {
      walk_state.v = 0.0f;
    }
  }

  move_rect(&player_data.rect, dt * walk_state.v, dt * jump_state.v);
}
