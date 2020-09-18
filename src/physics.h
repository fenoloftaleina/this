#include "math.h"


typedef struct
{
  const float gravity;
  const float init_v;
  const float init_double_v;
  float double_jump_threshold;


  float v;

  bool in_air;
  float started_at;
  bool possible_double_jump;
} jump_data;


typedef struct
{
  const float default_clamp;
  const float flight_clamp;
  const float damping;


  float v;
  float transpose;
  float clamp;
} walk_data;


static float e = 0.1f;


static jump_data jump_state = (jump_data){
  .gravity = -5.5f,
  .init_v = 1.085f,
  .init_double_v = 1.655f,
  .double_jump_threshold = 0.1f,

  .v = 0.0f,

  .in_air = true,
  .started_at = 0.0f,
  .possible_double_jump = false
};
static walk_data walk_state = (walk_data){
  .default_clamp = 0.6f,
  .flight_clamp = 0.42f,
  .damping = 20.0f,
  .v = 0.0f,
  .transpose = 6.5f
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
(collision_points_data* cp, const player_data* pd, const bool up, const bool left)
{
  bool down = !up;
  bool right = !left;

  if (left && down) {
    cp->hanging_x = pd->r.x1;
    cp->p_hanging_x = pd->pr.x1;
    cp->hanging_y = pd->r.y2;
    cp->p_hanging_y = pd->pr.y2;

    cp->dragging_x = pd->r.x2;
    cp->p_dragging_x = pd->pr.x2;
    cp->dragging_y = pd->r.y1;
    cp->p_dragging_y = pd->pr.y1;

    cp->main_x = pd->r.x1;
    cp->p_main_x = pd->pr.x1;
    cp->main_y = pd->r.y1;
    cp->p_main_y = pd->pr.y1;
  } else if (left && up) {
    cp->hanging_x = pd->r.x1;
    cp->p_hanging_x = pd->pr.x1;
    cp->hanging_y = pd->r.y1;
    cp->p_hanging_y = pd->pr.y1;

    cp->dragging_x = pd->r.x2;
    cp->p_dragging_x = pd->pr.x2;
    cp->dragging_y = pd->r.y2;
    cp->p_dragging_y = pd->pr.y2;

    cp->main_x = pd->r.x1;
    cp->p_main_x = pd->pr.x1;
    cp->main_y = pd->r.y2;
    cp->p_main_y = pd->pr.y2;
  } else if (right && down) {
    cp->hanging_x = pd->r.x2;
    cp->p_hanging_x = pd->pr.x2;
    cp->hanging_y = pd->r.y2;
    cp->p_hanging_y = pd->pr.y2;

    cp->dragging_x = pd->r.x1;
    cp->p_dragging_x = pd->pr.x1;
    cp->dragging_y = pd->r.y1;
    cp->p_dragging_y = pd->pr.y1;

    cp->main_x = pd->r.x2;
    cp->p_main_x = pd->pr.x2;
    cp->main_y = pd->r.y1;
    cp->p_main_y = pd->pr.y1;
  } else if (right && up) {
    cp->hanging_x = pd->r.x2;
    cp->p_hanging_x = pd->pr.x2;
    cp->hanging_y = pd->r.y1;
    cp->p_hanging_y = pd->pr.y1;

    cp->dragging_x = pd->r.x1;
    cp->p_dragging_x = pd->pr.x1;
    cp->dragging_y = pd->r.y2;
    cp->p_dragging_y = pd->pr.y2;

    cp->main_x = pd->r.x2;
    cp->p_main_x = pd->pr.x2;
    cp->main_y = pd->r.y2;
    cp->p_main_y = pd->pr.y2;
  }
}


void recalc_rect_side_end
(rect_side_end_data* rse, const rect* r, const bool up, const bool left)
{
  bool down = !up;
  bool right = !left;

  rse->side_y1 = r->y1;
  rse->side_y2 = r->y2;

  rse->end_x1 = r->x1;
  rse->end_x2 = r->x2;

  if (left && down) {
    rse->side_x = r->x2;
    rse->end_y = r->y2;
  } else if (left && up) {
    rse->side_x = r->x2;
    rse->end_y = r->y1;
  } else if (right && down) {
    rse->side_x = r->x1;
    rse->end_y = r->y2;
  } else if (right && up) {
    rse->side_x = r->x1;
    rse->end_y = r->y1;
  }
}


void check_collisions(player_data* pd, map_data* md)
{
  float eps = 0.001f;

  bool up = jump_state.v > 0;
  bool left = walk_state.v <= 0;
  bool down = !up;
  bool right = !left;

  float e_side = left ? eps : -eps;
  float e_end = up ? -eps : eps;

  collision_points_data cp;
  recalc_collision_points(&cp, pd, up, left);

  rect_side_end_data rse;

  for (int i = 0; i < md->n; ++i) {
    recalc_rect_side_end(&rse, &md->rs[i], up, left);
    if (lines_intersect(
          cp.p_hanging_x, cp.p_hanging_y, cp.hanging_x, cp.hanging_y,
          rse.side_x, rse.side_y1, rse.side_x, rse.side_y2)) {
      move_rect(&pd->r, rse.side_x - cp.hanging_x + e_side, 0.0f);
      recalc_collision_points(&cp, pd, up, left);
    }
  }

  for (int i = 0; i < md->n; ++i) {
    recalc_rect_side_end(&rse, &md->rs[i], up, left);
    if (lines_intersect(
          cp.p_dragging_x, cp.p_dragging_y, cp.dragging_x, cp.dragging_y,
          rse.end_x1, rse.end_y, rse.end_x2, rse.end_y)) {
      move_rect(&pd->r, 0.0f, rse.end_y - cp.dragging_y + e_end);
      recalc_collision_points(&cp, pd, up, left);

      jump_state.v = 0.0f;
      jump_state.in_air = up;
      jump_state.started_at = 0.0f;
      jump_state.possible_double_jump = false;
    }
  }

  for (int i = 0; i < md->n; ++i) {
    recalc_rect_side_end(&rse, &md->rs[i], up, left);
    if (lines_intersect(
          cp.p_main_x, cp.p_main_y, cp.main_x, cp.main_y,
          rse.side_x, rse.side_y1, rse.side_x, rse.side_y2)) {
      move_rect(&pd->r, rse.side_x - cp.main_x + e_side, 0.0f);
      recalc_collision_points(&cp, pd, up, left);
    }
  }

  for (int i = 0; i < md->n; ++i) {
    recalc_rect_side_end(&rse, &md->rs[i], up, left);
    if (lines_intersect(
          cp.p_main_x, cp.p_main_y, cp.main_x, cp.main_y,
          rse.end_x1, rse.end_y, rse.end_x2, rse.end_y)) {
      move_rect(&pd->r, 0.0f, rse.end_y - cp.main_y + e_end);
      recalc_collision_points(&cp, pd, up, left);

      jump_state.v = 0.0f;
      jump_state.in_air = up;
      jump_state.started_at = 0.0f;
      jump_state.possible_double_jump = false;
    }
  }
}


float v_clamp(const float val)
{
  return HMM_Clamp(-3.0f, val, 2.0f);
}

float h_clamp(const float val, const float clamp_val)
{
  return HMM_Clamp(-clamp_val, val, clamp_val);
}


void update_player_positions
(player_data* pd, const float t, const float dt, const input_data* in, map_data* md)
{
  // printf("%f\n", t);
  if (in->v == IN_UP && !jump_state.in_air) {
    jump_state.v = v_clamp(jump_state.init_v);
    jump_state.in_air = true;
    jump_state.started_at = t;
    jump_state.possible_double_jump = true;
  } else if (in->v == IN_UP &&
      jump_state.in_air &&
      jump_state.possible_double_jump &&
      t - jump_state.started_at > jump_state.double_jump_threshold
      ) {
    jump_state.v = v_clamp(jump_state.init_double_v);
    jump_state.possible_double_jump = false;
  } else if (t - jump_state.started_at > jump_state.double_jump_threshold) {
    jump_state.v = v_clamp(jump_state.v + dt * jump_state.gravity);
  }

  if (in->v != IN_UP) {
    jump_state.possible_double_jump = false;
  }

  if (jump_state.in_air) {
    walk_state.clamp = walk_state.flight_clamp;
  } else {
    walk_state.clamp = walk_state.default_clamp;
  }

  if (in->h == IN_LEFT) {
    walk_state.v = h_clamp(walk_state.v - dt * walk_state.transpose, walk_state.clamp);
  } else if (in->h == IN_RIGHT) {
    walk_state.v = h_clamp(walk_state.v + dt * walk_state.transpose, walk_state.clamp);
  } else {
    walk_state.v /= (1.0f + walk_state.damping * dt);
    if (fabs(walk_state.v) < e) {
      walk_state.v = 0.0f;
    }
  }

  move_rect(&pd->r, dt * walk_state.v, dt * jump_state.v);
}
