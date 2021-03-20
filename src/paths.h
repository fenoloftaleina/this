const int PATHS_M = 20;

typedef struct
{
  int i;
  int j;
} ij_t;

typedef struct path_data_t
{
  int length;
  int direction;
  int step;
  bool looped;
  bool self_only;

  ij_t positions[PATHS_M];
} path_data_t;


void reset_path(path_data_t* path_data)
{
  path_data->length = -1;
  path_data->self_only = false;
  memset(path_data->positions, -1, PATHS_M * sizeof(ij_t));
}


void init_path(path_data_t* path_data)
{
  reset_path(path_data);
}


void add_to_path(path_data_t* path_data, int i, int j)
{
  path_data->positions[path_data->length].i = i;
  path_data->positions[path_data->length].j = j;

  path_data->length += 1;
}


void restart_path(path_data_t* path_data, int i, int j)
{
  path_data->length = 0;
  path_data->direction = 1;
  path_data->step = 0;
  path_data->looped = false;
  path_data->self_only = false;

  add_to_path(path_data, i, j);
}


static pos_t temp_lines_positions[PATHS_M + 1];

void draw_path(path_data_t* path_data)
{
  if (path_data->length < 2) {
    return;
  }

  lines_data.thickness = 4;

  for (int j = 0; j < path_data->length; ++j) {
    temp_lines_positions[j] = (pos_t){
      path_data->positions[j].i * map_data.raw_tile_width +
        map_data.raw_tile_width * 0.5f,
      path_data->positions[j].j * map_data.raw_tile_height +
        map_data.raw_tile_height * 0.5f
    };
  }

  if (path_data->looped) {
    temp_lines_positions[path_data->length] = temp_lines_positions[0];
  }

  add_lines(
      &lines_bo, temp_lines_positions, path_data->length + path_data->looped,
      &(col_t){0.7f, 0.1f, 0.4f, 1.0f}, flat_z, false
      );
}


void advance_path
(path_data_t* path_data, rect_animation_t* rect_animation, const float t)
{
  float x1, y1, x2, y2;

  ij_to_xy(
      path_data->positions[path_data->step].i,
      path_data->positions[path_data->step].j,
      &x1, &y1, &x2, &y2
      );
  rect start_rect = (rect){
    x1, y1, x2, y2, 1.0f, 1.0f, 1.0f, 1.0f, flat_z
  };

  if (path_data->direction == 1) {
    if (path_data->step + 1 < path_data->length) {
      path_data->step += 1;
    } else {
      if (path_data->looped) {
        path_data->step = 0;
      } else {
        path_data->step -= 1;
        path_data->direction = -1;
      }
    }
  } else {
    if (path_data->step - 1 >= 0) {
      path_data->step -= 1;
    } else {
      if (path_data->looped) {
        path_data->step = path_data->length - 1;
      } else {
        path_data->step += 1;
        path_data->direction = 1;
      }
    }
  }

  ij_to_xy(
      path_data->positions[path_data->step].i,
      path_data->positions[path_data->step].j,
      &x1, &y1, &x2, &y2
      );
  rect end_rect = (rect){
    x1, y1, x2, y2, 1.0f, 1.0f, 1.0f, 1.0f, flat_z
  };
  float path_step_time = 1.0f;
  schedule_rect_animation(
      rect_animation,
      t,
      path_step_time,
      start_rect,
      end_rect,
      lerp_tween
      );
}
