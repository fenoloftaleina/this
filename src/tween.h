typedef struct
{
  float start_t;
  float end_t;
  float start_v;
  float end_v;

  float v;

  float (*fn)(const float, const float, const float, const float, const float);
} tween_data_t;


float lerp_tween
(const float t, const float start_t, const float end_t, const float start_v, const float end_v)
{
  float a = (t - start_t) / (end_t - start_t);

  if (a < 0.0f) {
    a = 0.0f;
  } else if (a > 1.0f) {
    a = 1.0f;
  }

  return start_v * (1.0f - a) + end_v * a;
}

float parabola
(const float t, const float start_t, const float end_t, const float start_v, const float end_v)
{
  float a = (t - start_t) / (end_t - start_t);

  if (a < 0.0f) {
    a = 0.0f;
  } else if (a > 1.0f) {
    a = 1.0f;
  }

  return start_v + pow(4.0f * a * (1.0f - a), 4.0f) * end_v;
}


float minus_pow_x2
(const float t, const float start_t, const float end_t, const float start_v, const float end_v)
{
  float a = (t - start_t) / (end_t - start_t);

  if (a < 0.0f) {
    a = 0.0f;
  } else if (a > 1.0f) {
    a = 1.0f;
  }

  return start_v + (- pow(a * 2.0f - 1.0f, 2.0f) + 1.0f) * end_v;
}


const float potential_frame_time_offset = dt;


void update_tween(tween_data_t* tween_data, const float t)
{
  if (t >= tween_data->start_t && t < tween_data->end_t + potential_frame_time_offset) {
    tween_data->v =
      tween_data->fn(
          t,
          tween_data->start_t,
          tween_data->end_t,
          tween_data->start_v,
          tween_data->end_v
          );
  }
}
