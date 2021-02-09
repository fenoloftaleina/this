static const float keyframe_time = dt * 10.0f;

typedef struct
{
  int frame;
  int n;
  float last_t;
} keyframes_data_t;


typedef struct
{
  int start_sprite_id;
  keyframes_data_t keyframes;

  int cur_sprite_id;
} animation_data_t;


void reset_animation(animation_data_t* animation_data, const float t)
{
  animation_data->keyframes.frame = 0;
  animation_data->keyframes.last_t = t;
}


void init_animation(animation_data_t* animation_data, int sprite_id, int n)
{
  reset_animation(animation_data, 0);
  animation_data->keyframes.n = n;
  animation_data->start_sprite_id = sprite_id;
  animation_data->cur_sprite_id = sprite_id;
}


void update_animation(animation_data_t* animation_data, const float t)
{
  int keyframes_diff = (int)((t - animation_data->keyframes.last_t) / keyframe_time);

  if (keyframes_diff > 0) {
    animation_data->keyframes.frame =
      (animation_data->keyframes.frame + keyframes_diff) % animation_data->keyframes.n;
    animation_data->keyframes.last_t += keyframes_diff * keyframe_time;
  }

  animation_data->cur_sprite_id =
    animation_data->start_sprite_id + animation_data->keyframes.frame;
}
