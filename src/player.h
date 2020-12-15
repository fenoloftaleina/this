typedef struct
{
  buffer_object bo;
  rect rect;
  rect prev_rect;
  float width;
  float height;

  bool just_jumped;
} player_data;


#include "physics.h"


void init_player(player_data* player)
{
  init_rects_buffer_object(&player->bo, 1);

  player->width = 100.0f / sapp_width();
  player->height = 120.0f / sapp_height();

  const float pw2 = player->width * 0.5f;
  const float ph2 = player->height * 0.5f;

  player->rect = (rect){-pw2, -ph2, pw2, ph2, 0.3f, 0.3f, 0.3f, 1.0f};
  move_rect(&player->rect, -0.05f, 0.3f);

  rects_write_vertices_simple(&player->rect, &player->bo, 1);
  rects_write_indices(&player->bo, 1);
  // update_buffer_vertices(&player->bo);
  update_buffer_indices(&player->bo);

  player->just_jumped = false;
}


void draw_player(player_data* player, const float frame_fraction)
{
  rects_write_vertices(&player->prev_rect, &player->rect, &player->bo, 1, frame_fraction);
  update_buffer_vertices(&player->bo);

  draw_buffer_object(&player->bo);
}
