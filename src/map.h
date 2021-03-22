#include "mpack.h"


typedef enum spot_type_status
{
  spot_inactive,
  spot_active
} spot_type_status;


typedef enum spot_type
{
  spot_empty = -1,
  spot_neutral,
  spot_one,
  spot_two,
  spot_three,
  spot_four,
  spot_five,
  spot_checkpoint,
  spot_spikes,
} spot_type;

const int SPRITE_OFFSET = 0;

const int spot_type_n = 9;


typedef struct path_data_t path_data_t;


typedef struct
{
  int n;
  rect* prev_rects; // prev
  rect* rects;
  spot_type* spot_types;
  spot_type_status spot_type_statuses[spot_type_n];

  int* matrix; // matrix, linear, but 2d
  int* next_matrix;

  float raw_tile_width, raw_tile_height;

  const int matrix_w;
  const int matrix_h;
  const int matrix_size;

  bool matrix_changed;

  // int offset_x, offsey_y; // for moving between maps potentially

  int* temp_models_list;

  tween_data_t tween_per_type[spot_type_n];
  schedule_data_t reset_schedule;

  path_data_t* paths;
  rect_animation_t* rect_animations;

  int player_start_x, player_start_y;
} map_data_t;


map_data_t map_data = (map_data_t){
  .matrix_w = 20,
  .matrix_h = 12,
  .matrix_size = 20 * 12
};


typedef struct color
{
  float r, g, b;
} color;


color type_colors[] = {
  {0.9f, 0.9f, 0.9f},
  {0.039f, 0.035f, 0.031f},
  {0.937f, 0.780f, 0.887f},
  {0.631f, 0.853f, 0.846f},
  {0.721f, 0.887f, 0.631f},
  {0.887f, 0.778f, 0.639f},
  {0.7f, 0.7f, 0.7f},
};

const color death_type_color = (color){0.8f, 0.8f, 0.8f};
const color checkpoint_type_color = (color){0.9f, 0.9f, 0.9f};
const color spikes_type_color = (color){0.9f, 0.3f, 0.2f};


float tile_width = 200.0f;
float tile_height = 200.0f;


void ij_to_xy(const int i, const int j, float* x1, float* y1, float* x2, float* y2)
{
  *x1 = -1.0f + i * map_data.raw_tile_width;
  *y1 = -1.0f + j * map_data.raw_tile_height;
  *x2 = *x1 + map_data.raw_tile_width;
  *y2 = *y1 + map_data.raw_tile_height;
}


void ii_to_ij(const int ii, int* i, int* j)
{
  *i = ii % map_data.matrix_w;
  *j = ii / map_data.matrix_w;
}


int ij_to_ii(int i, int j)
{
  return j * map_data.matrix_w + i;
}


void ii_to_xy(const int ii, float* x1, float* y1, float* x2, float* y2)
{
  int i, j;
  ii_to_ij(ii, &i, &j);
  ij_to_xy(i, j, x1, y1, x2, y2);
}


void rect_to_ij(const rect* rect, int* i, int* j)
{
  *i = ((rect->x1 + rect->x2) * 0.5f + 1.0f) / map_data.raw_tile_width;
  *j = ((rect->y1 + rect->y2) * 0.5f + 1.0f) / map_data.raw_tile_height;
}


void ii_to_jj_rect(const int ii, const int jj)
{
  float x1, y1, x2, y2;
  ii_to_xy(ii, &x1, &y1, &x2, &y2);

  map_data.rects[jj] = (rect){
    x1, y1, x2, y2, 1.0f, 1.0f, 1.0f, 1.0f, flat_z
  };
  map_data.prev_rects[jj] = map_data.rects[jj];

  set_sprite(&map_data.rects[jj], &texture, SPRITE_OFFSET + map_data.spot_types[jj]);

  reset_rect_animation(&map_data.rect_animations[jj], &map_data.rects[jj]);
}




#include "paths.h"




void reset_map()
{
  for (int i = 0; i < spot_type_n; ++i) {
    map_data.tween_per_type[i].fn = lerp_tween;
    map_data.tween_per_type[i].start_t = 0.0f;
    map_data.tween_per_type[i].end_t = 1.0f;
    map_data.tween_per_type[i].start_v = 0.0f;
    map_data.tween_per_type[i].end_v = 1.0f;

    map_data.spot_type_statuses[i] = spot_active;
  }

  map_data.tween_per_type[spot_checkpoint].fn = parabola_tween;

  reset_schedule(&map_data.reset_schedule);

  map_data.matrix_changed = false;
}


void init_map()
{
  map_data.raw_tile_width = tile_width;
  map_data.raw_tile_height = tile_height;

  map_data.prev_rects = (rect*)malloc(map_data.matrix_size * sizeof(rect));
  map_data.rects = (rect*)malloc(map_data.matrix_size * sizeof(rect));
  map_data.spot_types = (spot_type*)malloc(map_data.matrix_size * sizeof(spot_type));
  map_data.matrix = (int*)malloc(map_data.matrix_size * sizeof(int));
  memset(map_data.matrix, -1, map_data.matrix_size * sizeof(int));
  map_data.next_matrix = (int*)malloc(map_data.matrix_size * sizeof(int));
  map_data.temp_models_list = (int*)malloc(map_data.matrix_size * sizeof(int));
  map_data.paths = (path_data_t*)malloc(map_data.matrix_size * sizeof(path_data_t));
  map_data.rect_animations = (rect_animation_t*)malloc(map_data.matrix_size * sizeof(rect_animation_t));

  for (int i = 0; i < map_data.matrix_size; ++i) {
    init_rect_animation(&map_data.rect_animations[i]);
    init_path(&map_data.paths[i]);
  }

  reset_map();
}


static const float black_f = 0.1f;

void draw_map(const float frame_fraction)
{
  spot_type type;

  for (int i = 0; i < map_data.n; ++i) {
    type = map_data.spot_types[i];
    if (map_data.spot_type_statuses[type] == spot_active) {
      map_data.rects[i].z = map_data.prev_rects[i].z = flat_z - 0.3f;
    } else {
      map_data.rects[i].z = map_data.prev_rects[i].z = flat_z - 0.6f;
    }

    if (type == spot_checkpoint) {
      map_data.prev_rects[i].r = map_data.rects[i].r =
        lerp(
            type_colors[type].r,
            checkpoint_type_color.r,
            map_data.tween_per_type[type].v
            );
      map_data.prev_rects[i].g = map_data.rects[i].g =
        lerp(
            type_colors[type].g,
            checkpoint_type_color.g,
            map_data.tween_per_type[type].v
            );
      map_data.prev_rects[i].b = map_data.rects[i].b =
        lerp(
            type_colors[type].b,
            checkpoint_type_color.b,
            map_data.tween_per_type[type].v
            );

      map_data.temp_models_list[i] = 3;
    } else if (type == spot_spikes) {
      map_data.rects[i].r = map_data.prev_rects[i].r = spikes_type_color.r;
      map_data.rects[i].g = map_data.prev_rects[i].g = spikes_type_color.g;
      map_data.rects[i].b = map_data.prev_rects[i].b = spikes_type_color.b;

      map_data.temp_models_list[i] = 1;
    } else {
      map_data.rects[i].r = map_data.rects[i].g = map_data.rects[i].b =
        map_data.prev_rects[i].r = map_data.prev_rects[i].g = map_data.prev_rects[i].b =
        lerp(death_type_color.r, black_f, map_data.tween_per_type[type].v);

      map_data.temp_models_list[i] = 1;
    }
  }
  add_models(
      &rects_bo, map_data.temp_models_list, map_data.n, 1.0f,
      map_data.rects, map_data.prev_rects, frame_fraction
      );


  for (int i = 0; i < map_data.n; ++i) {
    type = map_data.spot_types[i];

    if (type == spot_neutral || type == spot_checkpoint || type == spot_spikes) {
      continue;
    }

    if (map_data.spot_type_statuses[type] == spot_active) {
      map_data.rects[i].z = map_data.prev_rects[i].z = flat_z - 0.3f;
    } else {
      map_data.rects[i].z = map_data.prev_rects[i].z = flat_z - 0.6f;
    }

    map_data.rects[i].r = map_data.rects[i].g = map_data.rects[i].b =
      map_data.prev_rects[i].r = map_data.prev_rects[i].g = map_data.prev_rects[i].b =
      lerp(death_type_color.r, black_f, map_data.tween_per_type[type].v);
    map_data.temp_models_list[i] = 2;
  }
  add_models(
      &rects_bo, map_data.temp_models_list, map_data.n, 1.0f,
      map_data.rects, map_data.prev_rects, frame_fraction
      );

  for (int i = 0; i < map_data.n; ++i) {
    type = map_data.spot_types[i];

    if (type == spot_neutral || type == spot_checkpoint || type == spot_spikes) {
      continue;
    }

    map_data.prev_rects[i].r = map_data.rects[i].r =
      lerp(
          death_type_color.r,
          type_colors[type].r,
          map_data.tween_per_type[type].v
          );
    map_data.prev_rects[i].g = map_data.rects[i].g =
      lerp(
          death_type_color.g,
          type_colors[type].g,
          map_data.tween_per_type[type].v
          );
    map_data.prev_rects[i].b = map_data.rects[i].b =
      lerp(
          death_type_color.b,
          type_colors[type].b,
          map_data.tween_per_type[type].v
          );

    map_data.temp_models_list[i] = 0;
  }
  add_models(
      &rects_bo, map_data.temp_models_list, map_data.n, 1.0f,
      map_data.rects, map_data.prev_rects, frame_fraction
      );

  for (int i = 0; i < map_data.n; ++i) {
    draw_path(&map_data.paths[i]);
  }
}


void matrix_to_rects()
{
  int jj = 0;
  for(int ii = 0; ii < map_data.matrix_size; ++ii) {
    jj = map_data.matrix[ii];

    if (jj == -1) {
      continue;
    }

    ii_to_jj_rect(ii, jj);
  }
}


void update_map(const float t)
{
  memset(map_data.next_matrix, -1, map_data.matrix_size * sizeof(int));

  int i, j, ii;
  map_data.matrix_changed = false;
  for (int jj = 0; jj < map_data.n; ++jj) {
    rect_to_ij(&map_data.rects[jj], &i, &j);
    ii = ij_to_ii(i, j);
    map_data.next_matrix[ii] = jj;

    if (map_data.matrix[ii] != map_data.next_matrix[ii]) {
      map_data.matrix_changed = true;
    }
  }

  int* matrix_p = map_data.matrix;
  map_data.matrix = map_data.next_matrix;
  map_data.next_matrix = matrix_p;


  for (int i = 0; i < spot_type_n; ++i) {
    update_tween(&map_data.tween_per_type[i], t);
  }

  execute_schedule(&map_data.reset_schedule, t);

  for (int i = 0; i < map_data.n; ++i) {
    update_rect_animation(&map_data.rect_animations[i], t);
  }

  memcpy(map_data.prev_rects, map_data.rects, map_data.n * sizeof(rect));
}


void load_map(const char* map_filename)
{
  char file_path[255];
  sprintf(file_path, "../../../main/src/maps/%s", map_filename);
  mpack_reader_t reader;
  mpack_reader_init_filename(&reader, file_path);

  mpack_tag_t tag;
  tag = mpack_read_tag(&reader);
  if (mpack_reader_error(&reader) != mpack_ok) {
    fprintf(stderr, "An error occurred decoding the data!\n");
    return;
  }

  // int a;
  // int j = 0;
  // for (int i = 0; i < map_data.matrix_size; ++i) {
  //   tag = mpack_read_tag(&reader);
  //   if (mpack_reader_error(&reader) != mpack_ok) {
  //     fprintf(stderr, "An error occurred decoding the data!\n");
  //     return;
  //   }
  //   if (mpack_tag_type(&tag) == mpack_type_uint) {
  //     a = mpack_tag_uint_value(&tag);
  //   } else {
  //     a = mpack_tag_int_value(&tag);
  //   }
  //   // printf("%d %d %d\n", i, j, a);
  //   if (a != -1) {
  //     map_data.matrix[i] = j;
  //     map_data.spot_types[j] = a;
  //     j += 1;
  //   } else {
  //     map_data.matrix[i] = -1;
  //   }
  // }
  // mpack_done_array(&reader);
  // map_data.n = j;

  // int cnt = mpack_tag_array_count(&tag);
  for (int i = 0; i < map_data.matrix_size; ++i) {
    tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map_data.matrix[i] = mpack_tag_uint_value(&tag);
    } else {
      map_data.matrix[i] = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
  }
  mpack_done_array(&reader);

  tag = mpack_read_tag(&reader);
  int cnt = mpack_tag_array_count(&tag);
  map_data.n = cnt;
  for (int i = 0; i < cnt; ++i) {
    tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map_data.spot_types[i] = mpack_tag_uint_value(&tag);
    } else {
      map_data.spot_types[i] = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
  }
  mpack_done_array(&reader);


  tag = mpack_read_tag(&reader);
  for (int i = 0; i < map_data.n; ++i) {
    tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map_data.paths[i].length = mpack_tag_uint_value(&tag);
    } else {
      map_data.paths[i].length = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }

    tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map_data.paths[i].direction = mpack_tag_uint_value(&tag);
    } else {
      map_data.paths[i].direction = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }

    tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map_data.paths[i].step = mpack_tag_uint_value(&tag);
    } else {
      map_data.paths[i].step = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }

    tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map_data.paths[i].looped = mpack_tag_uint_value(&tag);
    } else {
      map_data.paths[i].looped = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }

    for (int j = 0; j < PATHS_M; ++j) {
      tag = mpack_read_tag(&reader);
      if (mpack_reader_error(&reader) != mpack_ok) {
        fprintf(stderr, "An error occurred decoding the data!\n");
        return;
      }
      if (mpack_tag_type(&tag) == mpack_type_uint) {
        map_data.paths[i].positions[j].i = mpack_tag_uint_value(&tag);
      } else {
        map_data.paths[i].positions[j].i = mpack_tag_int_value(&tag);
      }
      if (mpack_reader_error(&reader) != mpack_ok) {
        fprintf(stderr, "An error occurred decoding the data!\n");
        return;
      }

      tag = mpack_read_tag(&reader);
      if (mpack_reader_error(&reader) != mpack_ok) {
        fprintf(stderr, "An error occurred decoding the data!\n");
        return;
      }
      if (mpack_tag_type(&tag) == mpack_type_uint) {
        map_data.paths[i].positions[j].j = mpack_tag_uint_value(&tag);
      } else {
        map_data.paths[i].positions[j].j = mpack_tag_int_value(&tag);
      }
      if (mpack_reader_error(&reader) != mpack_ok) {
        fprintf(stderr, "An error occurred decoding the data!\n");
        return;
      }
    }
  }
  mpack_done_array(&reader);


  tag = mpack_read_tag(&reader);
  if (mpack_reader_error(&reader) != mpack_ok) {
    fprintf(stderr, "An error occurred decoding the data!\n");
    return;
  }
  if (mpack_tag_type(&tag) == mpack_type_uint) {
    map_data.player_start_x = mpack_tag_uint_value(&tag);
  } else {
    map_data.player_start_x = mpack_tag_int_value(&tag);
  }

  tag = mpack_read_tag(&reader);
  if (mpack_reader_error(&reader) != mpack_ok) {
    fprintf(stderr, "An error occurred decoding the data!\n");
    return;
  }
  if (mpack_tag_type(&tag) == mpack_type_uint) {
    map_data.player_start_y = mpack_tag_uint_value(&tag);
  } else {
    map_data.player_start_y = mpack_tag_int_value(&tag);
  }


  reset_map();

  matrix_to_rects();
}


void save_map(const char* map_filename)
{
  char file_path[255];
  sprintf(file_path, "%ssrc/maps/%s", main_dir, map_filename);
  mpack_writer_t writer;
  mpack_writer_init_filename(&writer, file_path);
  mpack_start_array(&writer, map_data.matrix_size);
  for (int i = 0; i < map_data.matrix_size; ++i) {
    mpack_write_int(&writer, map_data.matrix[i]);
  }
  mpack_finish_array(&writer);
  mpack_start_array(&writer, map_data.n);
  for (int i = 0; i < map_data.n; ++i) {
    mpack_write_int(&writer, map_data.spot_types[i]);
  }
  mpack_finish_array(&writer);
  mpack_start_array(&writer, map_data.n * (4 + PATHS_M * 2));
  for (int i = 0; i < map_data.n; ++i) {
    mpack_write_int(&writer, map_data.paths[i].length);
    mpack_write_int(&writer, map_data.paths[i].direction);
    mpack_write_int(&writer, map_data.paths[i].step);
    mpack_write_int(&writer, map_data.paths[i].looped);

    for (int j = 0; j < PATHS_M; ++j) {
      mpack_write_int(&writer, map_data.paths[i].positions[j].i);
      mpack_write_int(&writer, map_data.paths[i].positions[j].j);
    }
  }
  mpack_finish_array(&writer);
  mpack_write_int(&writer, map_data.player_start_x);
  mpack_write_int(&writer, map_data.player_start_y);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    fprintf(stderr, "An error occurred encoding the data!\n");
    return;
  }
}


void set_ij_spot(const int i, const int j, const spot_type t)
{
  int ii = ij_to_ii(i, j);
  int jj = map_data.matrix[ii];

  if (jj == -1) {
    map_data.matrix[ii] = map_data.n;
    jj = map_data.matrix[ii];
    map_data.n += 1;
  }

  map_data.spot_types[jj] = t;

  ii_to_jj_rect(ii, jj);
}


void move_ij_spot_to_ij(const int i1, const int j1, const int i2, const int j2)
{
  int ii1 = ij_to_ii(i1, j1);
  int ii2 = ij_to_ii(i2, j2);

  int jj = map_data.matrix[ii1];

  map_data.matrix[ii1] = -1;
  map_data.matrix[ii2] = jj;

  ii_to_jj_rect(ii2, jj);
}


void remove_ij_spot(const int i, const int j)
{
  int ii = ij_to_ii(i, j);
  int jj = map_data.matrix[ii];

  if (jj == -1) {
    return;
  }

  int elements = map_data.n - jj - 1;

  memmove(map_data.spot_types + jj, map_data.spot_types + jj + 1, elements * sizeof(spot_type));
  memmove(map_data.rects + jj, map_data.rects + jj + 1, elements * sizeof(rect));
  memmove(map_data.prev_rects + jj, map_data.prev_rects + jj + 1, elements * sizeof(rect));

  for (int i = 0; i < map_data.matrix_size; ++i) {
    if (map_data.matrix[i] >= jj) {
      map_data.matrix[i] -= 1;
    }
  }

  map_data.n -= 1;

  map_data.matrix[ii] = -1;
}


spot_type get_ij_spot(const int i, const int j)
{
  int ii = ij_to_ii(i, j);
  int jj = map_data.matrix[ii];

  if (jj == -1) {
    return -1;
  }

  return map_data.spot_types[jj];
}
