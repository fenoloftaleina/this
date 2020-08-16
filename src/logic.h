typedef struct
{
  int touch_ids[2];

  const int default_steps_till_eval;
  int steps_till_eval;
} logic_data;


void reload_logic(logic_data* ld)
{
  ld->steps_till_eval = ld->default_steps_till_eval;
}


void update
(player_data* pd, const float t, const float dt, const input_data* in, map_data* md, logic_data* ld)
{
  md->changed = false;
  pd->pr = pd->r;

  update_player_positions(pd, t, dt, in, md);
  check_collisions(pd, md);

  // figure out what is touched
}
