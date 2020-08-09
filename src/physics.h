#include "math.h"


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


static bool in_air = false;

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
        pd->vy = 0.0f;
        in_air = false;
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
        pd->vy = 0.0f;
        printf("%f %f %f\n", diff_y, col_y, pd->r.y2);
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
  return HMM_Clamp(-3.0f, val, 20.0f);
}

float h_clamp(const float val)
{
  return HMM_Clamp(-1.0f, val, 1.0f);
}


static float gravity = -5.5f,
             jump_v = 1.5f,
             move_x = 8.5f,
             cur_move_x = move_x,
             damping = 20.0f,
             e = 0.1f;

void update_player_positions
(player_data* pd, const float dt, const input_data* in, map_data* md)
{
  if (in->v == IN_UP && !in_air) {
    pd->vy = v_clamp(pd->vy + jump_v);
    in_air = true;
  } else {
    pd->vy = v_clamp(pd->vy + dt * gravity);
  }

  if (in->h == IN_LEFT) {
    pd->vx = h_clamp(pd->vx - dt * cur_move_x);
  } else if (in->h == IN_RIGHT) {
    pd->vx = h_clamp(pd->vx + dt * cur_move_x);
  } else {
    pd->vx /= (1.0f + damping * dt);
    if (abs(pd->vx) < e) {
      pd->vx = 0.0f;
    }
  }

  move_rect(&pd->r, dt * pd->vx, dt * pd->vy);

  check_collisions(pd, md);
}
