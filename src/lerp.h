typedef struct
{
  float start_t;
  float end_t;
  float start_v;
  float end_v;

  float cur_v;
} lerp_state;


void calc_lerp_cur_vs(const int n, lerp_state* lerp, const float t)
{
  for (int i = 0; i < n; ++i) {
    float a = (t - lerp[i].start_t) / (lerp[i].end_t - lerp[i].start_t);

    if (a < 0.0f) {
      a = 0.0f;
    } else if (a > 1.0f) {
      a = 1.0f;
    }

    lerp[i].cur_v = lerp[i].start_v * (1.0f - a) + lerp[i].end_v * a;
  }
}
