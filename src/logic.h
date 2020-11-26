typedef struct
{
  int touch_ids[3];
  spot_type touch_spot_types[3];
  bool jumped_meantime;

  const int default_steps_till_eval;
  int steps_till_eval;
  int n;
} logic_data;


void reload_logic(logic_data* ld)
{
  ld->steps_till_eval = ld->default_steps_till_eval;
  ld->n = 0;
  ld->jumped_meantime = false;
  for (int i = 0; i < ld->steps_till_eval; ++i) {
    ld->touch_ids[i] = -1;
  }
}


void kill_spots(const int id, map_data* md)
{
  spot_type type = md->ts[id];

  for (int i = 0; i < md->n; ++i) {
    if (md->ts[i] == type) {
      md->ss[i] = spot_dead;

      md->rs[i].r = dead_type_colors[type].r;
      md->rs[i].g = dead_type_colors[type].g;
      md->rs[i].b = dead_type_colors[type].b;
    }
  }
}


void reset_spots(map_data* md)
{
  for (int i = 0; i < md->n; ++i) {
    md->ss[i] = spot_alive;

    md->rs[i].r = type_colors[md->ts[i]].r;
    md->rs[i].g = type_colors[md->ts[i]].g;
    md->rs[i].b = type_colors[md->ts[i]].b;
  }
}


void evaluate(player_data* pd, map_data* md, logic_data* ld)
{

  ld->n = 0;
}


void update
(player_data* pd, const float t, const float dt, const input_data* in, map_data* md, logic_data* ld)
{
  pd->just_jumped = false;

  update_player_positions(pd, t, dt, in, md);

  if (pd->just_jumped) {
    ld->jumped_meantime = true;
    printf("jumped meantime\n\n");
  }

  check_collisions(pd, md);


  float eps = 0.0011f;

  int bottom_id = -1;
  spot_type bottom_spot_type = -1;
  float bottom_overlap = 0.0f;
  int top_id = -1;
  spot_type top_spot_type = -1;
  float top_overlap = 0.0f;
  int left_id = -1;
  spot_type left_spot_type = -1;
  float left_overlap = 0.0f;
  int right_id = -1;
  spot_type right_spot_type = -1;
  float right_overlap = 0.0f;

  float temp_overlap;
  int found_id = -1;

  for (int i = 0; i < md->n; ++i) {
    if (pd->r.x1 < md->rs[i].x1) {
      temp_overlap = pd->r.x1 + pd->w - md->rs[i].x1;
    } else if (pd->r.x2 <= md->rs[i].x2) {
      temp_overlap = pd->w;
    } else {
      temp_overlap = md->rs[i].x2 - pd->r.x1;
    }

    if (temp_overlap > bottom_overlap &&
        fabs(pd->r.y1 - md->rs[i].y2) < eps) {
      bottom_id = i;
      found_id = i;
      bottom_spot_type = md->ts[i];
      bottom_overlap = temp_overlap;
    }

    if (temp_overlap > top_overlap &&
        fabs(md->rs[i].y1 - pd->r.y2) < eps) {
      top_id = i;
      found_id = i;
      top_spot_type = md->ts[i];
      top_overlap = temp_overlap;
    }


    if (pd->r.y1 < md->rs[i].y1) {
      temp_overlap = pd->r.y1 + pd->h - md->rs[i].y1;
    } else if (pd->r.y2 <= md->rs[i].y2) {
      temp_overlap = pd->w;
    } else {
      temp_overlap = md->rs[i].y2 - pd->r.y1;
    }

    if (temp_overlap > left_overlap &&
        fabs(pd->r.x1 - md->rs[i].x2) < eps) {
      left_id = i;
      found_id = i;
      left_spot_type = md->ts[i];
      left_overlap = temp_overlap;
    }

    if (temp_overlap > right_overlap &&
        fabs(md->rs[i].x1 - pd->r.x2) < eps) {
      right_id = i;
      found_id = i;
      right_spot_type = md->ts[i];
      right_overlap = temp_overlap;
    }
  }

  // sdtx_printf("%d. found_id: %d\n", ld->n, found_id);
  // printf("%d. found_id: %d -- %d << %d\n", ld->n, found_id, (ld->n + 2) % ld->steps_till_eval, ld->jumped_meantime);

  if (found_id == -1 ||
      (!ld->jumped_meantime &&
       ld->touch_ids[(ld->n + 2) % ld->steps_till_eval] == found_id)) {
    return;
  }

  ld->jumped_meantime = false;

  // printf("----- %d %d %d %d\n", bottom_id, top_id, left_id, right_id);

  if (bottom_id != -1 && top_id == -1 && left_id == -1 && right_id == -1) {
    ld->touch_ids[ld->n] = bottom_id;
    ld->touch_spot_types[ld->n] = bottom_spot_type;
    kill_spots(bottom_id, md);
    ld->n += 1;
  } else if (bottom_id == -1 && top_id != -1 && left_id == -1 && right_id == -1) {
    ld->touch_ids[ld->n] = top_id;
    ld->touch_spot_types[ld->n] = top_spot_type;
    kill_spots(top_id, md);
    ld->n += 1;
  } else if (bottom_id == -1 && top_id == -1 && left_id != -1 && right_id == -1) {
    ld->touch_ids[ld->n] = left_id;
    ld->touch_spot_types[ld->n] = left_spot_type;
    kill_spots(left_id, md);
    ld->n += 1;
  } else if (bottom_id == -1 && top_id == -1 && left_id == -1 && right_id != -1) {
    ld->touch_ids[ld->n] = right_id;
    ld->touch_spot_types[ld->n] = right_spot_type;
    kill_spots(right_id, md);
    ld->n += 1;
  }

  if (ld->n == ld->steps_till_eval) {
    evaluate(pd, md, ld);
    reset_spots(md);
    // printf("reset!\n");
    // sdtx_printf("reset!");
  }
}
