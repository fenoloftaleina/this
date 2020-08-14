typedef struct
{
  buffer_object bo;
  rect r;
  rect pr; // prev rect
  float vx, vy;
} player_data;


#include "physics.h"


static float player_width = 100.0f;
static float player_height = 120.0f;


void init_player(player_data* pd)
{
  init_buffer_object(&pd->bo, vertices_per_rect, indices_per_rect);

  const float pw2 = player_width / (2.0f * sapp_width());
  const float ph2 = player_height / (2.0f * sapp_height());

  pd->r = {-pw2, -ph2, pw2, ph2, 0.3f, 0.3f, 0.3f, 1.0f};
  move_rect(&pd->r, 0.0f, 0.3f);

  rects_write_vertices_simple(&pd->r, &pd->bo, 1);
  rects_write_indices(&pd->bo, 1);
  // update_buffer_vertices(&pd->bo);
  update_buffer_indices(&pd->bo);

  pd->vx = 0.0f;
  pd->vy = 0.0f;
}


void draw_player(player_data* pd, const float frame_fraction)
{
  rects_write_vertices(&pd->pr, &pd->r, &pd->bo, 1, frame_fraction);
  update_buffer_vertices(&pd->bo);

  draw_buffer_object(&pd->bo);
}
