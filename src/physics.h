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
  const float default_transpose;
  const float damping;


  float v;
  float transpose;
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
  .default_transpose = 6.5f,
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


void check_collisions(player_data* pd, map_data* md)
{
  float col_x, col_y, diff_x = 0.0f, diff_y = 0.0f;

  float w2 = (pd->r.x2 - pd->r.x1) * 0.5f;
  float pr_mid_x = pd->pr.x1 + w2;
  float r_mid_x = pd->r.x1 + w2;

  float eps = 0.001f;

  col_y = -1000.0f;
  // falling
  if (pd->vy <= 0) {
    for (int i = 0; i < md->n; ++i) {
      if (lines_intersect(pr_mid_x, pd->pr.y1, r_mid_x, pd->r.y1,
            md->rs[i].x1 - w2, md->rs[i].y2, md->rs[i].x2 + w2, md->rs[i].y2)) {
        col_y = fmax(col_y, md->rs[i].y2);
        diff_y = col_y - pd->r.y1 + eps;

        jump_state.v = 0.0f;
        jump_state.in_air = false;
        jump_state.started_at = 0.0f;
        jump_state.possible_double_jump = false;
      }
    }
  }

  col_y = 1000.0f;
  // flying up
  if (pd->vy > 0) {
    for (int i = 0; i < md->n; ++i) {
      if (lines_intersect(pr_mid_x, pd->pr.y2, r_mid_x, pd->r.y2,
            md->rs[i].x1 - w2, md->rs[i].y1, md->rs[i].x2 + w2, md->rs[i].y1)) {
        col_y = fmin(col_y, md->rs[i].y1);
        diff_y = col_y - pd->r.y2 - eps;

        jump_state.v = 0.0f;
        jump_state.started_at = 0.0f;
        jump_state.possible_double_jump = false;
      }
    }
  }

  move_rect(&pd->r, 0.0f, diff_y);

  float h2 = (pd->r.y2 - pd->r.y1) * 0.5f;
  float pr_mid_y = pd->pr.y1 + h2;
  float r_mid_y = pd->r.y1 + h2;

  col_x = -1000.0f;
  // left
  if (pd->vx <= 0) {
    for (int i = 0; i < md->n; ++i) {
      if (lines_intersect(pd->pr.x1, pr_mid_y, pd->r.x1, r_mid_y,
            md->rs[i].x2, md->rs[i].y1 - h2, md->rs[i].x2, md->rs[i].y2 + h2)) {
        col_x = fmax(col_x, md->rs[i].x2);
        diff_x = col_x - pd->r.x1 + eps;
      }
    }
  }

  col_x = 1000.0f;
  // right
  if (pd->vx > 0) {
    for (int i = 0; i < md->n; ++i) {
      if (lines_intersect(pd->pr.x2, pr_mid_y, pd->r.x2, r_mid_y,
            md->rs[i].x1, md->rs[i].y2 + h2, md->rs[i].x1, md->rs[i].y1 - h2)) {
        col_x = fmin(col_x, md->rs[i].x1);
        diff_x = col_x - pd->r.x2 - eps;
      }
    }
  }

  move_rect(&pd->r, diff_x, 0.0f);
}


float v_clamp(const float val)
{
  return HMM_Clamp(-3.0f, val, 50.0f);
}

float h_clamp(const float val)
{
  return HMM_Clamp(-1.0f, val, 1.0f);
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

  if (in->h == IN_LEFT) {
    walk_state.v = h_clamp(walk_state.v - dt * walk_state.transpose);
  } else if (in->h == IN_RIGHT) {
    walk_state.v = h_clamp(walk_state.v + dt * walk_state.transpose);
  } else {
    walk_state.v /= (1.0f + walk_state.damping * dt);
    if (fabs(walk_state.v) < e) {
      walk_state.v = 0.0f;
    }
  }

  move_rect(&pd->r, dt * walk_state.v, dt * jump_state.v);
}
