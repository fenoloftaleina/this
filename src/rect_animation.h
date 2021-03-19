static const int RECT_ANIMATION_N = 20;

typedef struct
{
  int schedule_i;
  int execute_i;
  float start_times[RECT_ANIMATION_N];
  float end_times[RECT_ANIMATION_N];
  rect* cur_rect_p;
  rect start_rects[RECT_ANIMATION_N];
  rect end_rects[RECT_ANIMATION_N];
  tween_data_t tweens[RECT_ANIMATION_N];

} rect_animation_t;


void set_rect_animation(rect_animation_t* rect_animation, rect* cur_rect_p)
{
  rect_animation->cur_rect_p = cur_rect_p;
}


void init_rect_animation(rect_animation_t* rect_animation)
{
  rect_animation->schedule_i = 0;
  rect_animation->execute_i = -1;
}


void schedule_rect_animation(
    rect_animation_t* rect_animation,
    const float start_time,
    const float duration,
    const rect start_rect,
    const rect end_rect,
    float (*tween_fn)(const float)
    )
{
  if (rect_animation->execute_i != -1 &&
      start_time < rect_animation->end_times[rect_animation->execute_i]) {
    rect_animation->start_times[rect_animation->schedule_i] =
      rect_animation->end_times[rect_animation->execute_i];
  } else {
    rect_animation->start_times[rect_animation->schedule_i] = start_time;
  }
  rect_animation->end_times[rect_animation->schedule_i] =
    rect_animation->start_times[rect_animation->schedule_i] + duration;

  rect_animation->start_rects[rect_animation->schedule_i] = start_rect;
  rect_animation->end_rects[rect_animation->schedule_i] = end_rect;

  rect_animation->tweens[rect_animation->schedule_i].fn = tween_fn;

  rect_animation->schedule_i = (rect_animation->schedule_i + 1) % RECT_ANIMATION_N;
}


void update_rect_animation(rect_animation_t* rect_animation, const float t)
{
  int i = (rect_animation->execute_i + 1) % RECT_ANIMATION_N;

  while (i != rect_animation->schedule_i) {
    if (rect_animation->start_times[i] > t) {
      break;
    }

    if (rect_animation->end_times[i] >= t) {
      rect_animation->tweens[i].start_t = rect_animation->start_times[i];
      rect_animation->tweens[i].end_t = rect_animation->end_times[i];
      rect_animation->tweens[i].start_v = 0.0f;
      rect_animation->tweens[i].end_v = 1.0f;

      rect_animation->execute_i = i;
      break;
    }

    i = (i + 1) % RECT_ANIMATION_N;
  }

  if (rect_animation->execute_i == -1) {
    return;
  }

  update_tween(&rect_animation->tweens[rect_animation->execute_i], t);

  rect_animation->cur_rect_p->x1 =
    rect_animation->start_rects[rect_animation->execute_i].x1 *
    (1.0f - rect_animation->tweens[rect_animation->execute_i].v) +
    rect_animation->end_rects[rect_animation->execute_i].x1 *
    rect_animation->tweens[rect_animation->execute_i].v;

  rect_animation->cur_rect_p->y1 =
    rect_animation->start_rects[rect_animation->execute_i].y1 *
    (1.0f - rect_animation->tweens[rect_animation->execute_i].v) +
    rect_animation->end_rects[rect_animation->execute_i].y1 *
    rect_animation->tweens[rect_animation->execute_i].v;

  rect_animation->cur_rect_p->x2 =
    rect_animation->start_rects[rect_animation->execute_i].x2 *
    (1.0f - rect_animation->tweens[rect_animation->execute_i].v) +
    rect_animation->end_rects[rect_animation->execute_i].x2 *
    rect_animation->tweens[rect_animation->execute_i].v;

  rect_animation->cur_rect_p->y2 =
    rect_animation->start_rects[rect_animation->execute_i].y2 *
    (1.0f - rect_animation->tweens[rect_animation->execute_i].v) +
    rect_animation->end_rects[rect_animation->execute_i].y2 *
    rect_animation->tweens[rect_animation->execute_i].v;
}
