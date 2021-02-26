typedef struct
{
  float start_t;
  float end_t;
  float start_v;
  float end_v;

  float v;

  float (*fn)(const float);
} tween_data_t;


float lerp_tween(const float a)
{
  return a;
}


float pcurve(float x, float a, float b)
{
    const float k = pow(a+b,a+b) / (pow(a,a)*pow(b,b));
    return k * pow( x, a ) * pow( 1.0-x, b );
}


float parabola_tween(const float a)
{
  return pow(4.0f * a * (1.0f - a), 4.0f);
}


float pcurve_tween(const float a)
{
  return pcurve(a, 0.5f, 0.7f);
}


float minus_pow_x2 (const float a)
{
  return -pow(a * 2.0f - 1.0f, 2.0f) + 1.0f;
}


const float potential_frame_time_offset = dt;


void update_tween(tween_data_t* tween_data, const float t)
{
  // if (t >= tween_data->start_t && t < tween_data->end_t + potential_frame_time_offset) {

  float a = (t - tween_data->start_t) / (tween_data->end_t - tween_data->start_t);

  if (a < 0.0f) {
    a = 0.0f;
  } else if (a > 1.0f) {
    a = 1.0f;
  }

  a = tween_data->fn(a);

  tween_data->v = tween_data->start_v * (1.0f -a ) + a * tween_data->end_v;
}
