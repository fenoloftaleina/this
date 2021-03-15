#include "mpack.h"


typedef enum spot_type
{
  spot_empty = -1,
  spot_neutral,
  spot_pushable,
  spot_spikes,
} spot_type;

typedef enum matrix_state
{
  state_empty = -1,
  state_neutral,
  state_will_die,
  state_will_be_pushed_up,
  state_will_be_pushed_down,
  state_will_be_pushed_left,
  state_will_be_pushed_right,
  state_push_impasse,
} matrix_state;


const int SPRITE_OFFSET = 0;

const int spot_type_n = 3;


typedef struct
{
  int n;
  rect* rects;
  rect* prev_rects; // prev
  spot_type* spot_types;

  int* matrix; // matrix, linear, but 2d
  matrix_state* states_matrix;

  float raw_tile_width, raw_tile_height;

  const int m_w;
  const int m_h;
  const int m;

  tween_data_t tween_per_type[spot_type_n];
  schedule_data_t reset_schedule;

  int player_start_x, player_start_y;
} map_data_t;


map_data_t map_data = (map_data_t){
  .m_w = 20,
  .m_h = 12,
  .m = 20 * 12
};


typedef struct color
{
  float r, g, b;
} color;


float tile_width = 200.0f;
float tile_height = 200.0f;


void reset_map()
{
  for (int i = 0; i < spot_type_n; ++i) {
    map_data.tween_per_type[i].fn = lerp_tween;

    map_data.tween_per_type[i].start_t = 0.0f;
    map_data.tween_per_type[i].end_t = 1.0f;
    map_data.tween_per_type[i].start_v = 0.0f;
    map_data.tween_per_type[i].end_v = 1.0f;
  }

  reset_schedule(&map_data.reset_schedule);
}


void init_map()
{
  map_data.raw_tile_width = tile_width;
  map_data.raw_tile_height = tile_height;

  map_data.prev_rects = (rect*)malloc(map_data.m * sizeof(rect));
  map_data.rects = (rect*)malloc(map_data.m * sizeof(rect));
  map_data.matrix = (int*)malloc(map_data.m * sizeof(int));
  memset(map_data.matrix, -1, map_data.m * sizeof(int));
  map_data.states_matrix = (matrix_state*)malloc(map_data.m * sizeof(matrix_state));
  map_data.spot_types = (spot_type*)malloc(map_data.m * sizeof(spot_type));

  reset_map();
}


static const float black_f = 0.1f;

void draw_map(const float frame_fraction)
{
  add_rects(
      &sprites_bo, map_data.rects, map_data.prev_rects, map_data.n, frame_fraction
      );
}


void ij_to_xy(const int i, const int j, float* x1, float* y1, float* x2, float* y2)
{
  *x1 = -1.0f + i * map_data.raw_tile_width;
  *y1 = -1.0f + j * map_data.raw_tile_height;
  *x2 = *x1 + map_data.raw_tile_width;
  *y2 = *y1 + map_data.raw_tile_height;
}


void ii_to_ij(const int ii, int* i, int* j)
{
  *i = ii % map_data.m_w;
  *j = ii / map_data.m_w;
}


int ij_to_ii(int i, int j)
{
  return j * map_data.m_w + i;
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
}


void matrix_to_rects()
{
  int jj = 0;
  int count = 0;
  for(int ii = 0; ii < map_data.m; ++ii) {
    jj = map_data.matrix[ii];

    if (jj == -1) {
      continue;
    }

    ii_to_jj_rect(ii, jj);

    count += 1;
  }

  map_data.n = count;
}


void update_map(const float t)
{
  for (int i = 0; i < spot_type_n; ++i) {
    update_tween(&map_data.tween_per_type[i], t);
  }

  execute_schedule(&map_data.reset_schedule, t);

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

  for (int i = 0; i < map_data.m; ++i) {
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
  if (mpack_tag_type(&tag) == mpack_type_uint) {
    map_data.player_start_x = mpack_tag_uint_value(&tag);
  } else {
    map_data.player_start_x = mpack_tag_int_value(&tag);
  }
  if (mpack_reader_error(&reader) != mpack_ok) {
    fprintf(stderr, "An error occurred decoding the data!\n");
    return;
  }

  tag = mpack_read_tag(&reader);
  if (mpack_tag_type(&tag) == mpack_type_uint) {
    map_data.player_start_y = mpack_tag_uint_value(&tag);
  } else {
    map_data.player_start_y = mpack_tag_int_value(&tag);
  }
  if (mpack_reader_error(&reader) != mpack_ok) {
    fprintf(stderr, "An error occurred decoding the data!\n");
    return;
  }


  reset_map();

  matrix_to_rects();
}


void save_map(const char* map_filename)
{
  char file_path[255];
  sprintf(file_path, "%ssrc/maps/%s", main_dir, map_filename);

  printf("SAVE %s\n", file_path);

  mpack_writer_t writer;
  mpack_writer_init_filename(&writer, file_path);
  mpack_start_array(&writer, map_data.m);
  for (int i = 0; i < map_data.m; ++i) {
    mpack_write_int(&writer, map_data.matrix[i]);
  }
  mpack_finish_array(&writer);
  mpack_start_array(&writer, map_data.n);
  for (int i = 0; i < map_data.n; ++i) {
    mpack_write_int(&writer, map_data.spot_types[i]);
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

  for (int i = 0; i < map_data.m; ++i) {
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
