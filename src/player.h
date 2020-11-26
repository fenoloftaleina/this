typedef struct
{
  buffer_object bo;
  rect r;
  rect pr; // prev rect
  float w;
  float h;

  bool just_jumped;
} player_data;


#include "physics.h"


void init_player(player_data* pd)
{
  init_rects_buffer_object(&pd->bo, 1);

  pd->w = 100.0f / sapp_width();
  pd->h = 120.0f / sapp_height();

  const float pw2 = pd->w * 0.5f;
  const float ph2 = pd->h * 0.5f;

  pd->r = (rect){-pw2, -ph2, pw2, ph2, 0.3f, 0.3f, 0.3f, 1.0f};
  move_rect(&pd->r, -0.05f, 0.3f);

  rects_write_vertices_simple(&pd->r, &pd->bo, 1);
  rects_write_indices(&pd->bo, 1);
  // update_buffer_vertices(&pd->bo);
  update_buffer_indices(&pd->bo);

  pd->just_jumped = false;
}


void draw_player(player_data* pd, const float frame_fraction)
{
  rects_write_vertices(&pd->pr, &pd->r, &pd->bo, 1, frame_fraction);
  update_buffer_vertices(&pd->bo);

  draw_buffer_object(&pd->bo);
}
