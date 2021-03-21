#include "mpack.h"


typedef enum spot_type_status
{
  spot_inactive,
  spot_active
} spot_type_status;


typedef enum spot_type
{
  spot_empty = -1,
  spot_chocolate,
  spot_shit,
  spot_shop,
  spot_car
} spot_type;

const int SPRITE_OFFSET = 0;

const int spot_type_n = 4;


typedef struct
{
  int n;
  rect* prev_rects; // prev
  rect* rects;
  spot_type* spot_types;
  spot_type_status spot_type_statuses[spot_type_n];

  int* matrix; // matrix, linear, but 2d

  float raw_tile_width, raw_tile_height;

  const int matrix_w;
  const int matrix_h;
  const int matrix_size;

  // int offset_x, offsey_y; // for moving between maps potentially

  int* temp_models_list;

  tween_data_t tween_per_type[spot_type_n];
  schedule_data_t reset_schedule;

  rect_animation_t* rect_animations;

  int player_start_x, player_start_y;


  animation_data_t animations[spot_type_n];
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




#include "paths.h"




void reset_map()
{
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
  map_data.temp_models_list = (int*)malloc(map_data.matrix_size * sizeof(int));

  for (int i = 0; i < spot_type_n; ++i) {
    init_animation(&map_data.animations[i], i * 2, 2);
  }

  reset_map();
}


static const float black_f = 0.1f;

void draw_map(const float frame_fraction)
{
  add_rects(&sprites_bo, map_data.rects, map_data.prev_rects, map_data.n, frame_fraction);
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

  set_sprite(&map_data.rects[jj], &texture, map_data.spot_types[jj] * 2);
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
  for (int i = 0; i < spot_type_n; ++i) {
    update_animation(&map_data.animations[i], t);
  }

  for (int i = 0; i < map_data.n; ++i) {
    // printf("set sprite for type %d on %d to %d\n", map_data.spot_types[i], i, map_data.animations[map_data.spot_types[i]].cur_sprite_id);
    set_sprite(&map_data.rects[i], &texture, map_data.animations[map_data.spot_types[i]].cur_sprite_id);
  }

  memcpy(map_data.prev_rects, map_data.rects, map_data.n * sizeof(rect));
}


void load_map(const char* map_filename)
{
  printf("load %s\n", map_filename);

  char file_path[255];
  sprintf(file_path, "../../../main/src/maps/%s", map_filename);
  mpack_reader_t reader;
  mpack_reader_init_filename(&reader, file_path);

  mpack_tag_t tag;
  tag = mpack_read_tag(&reader);
  if (mpack_reader_error(&reader) != mpack_ok) {
    fprintf(stderr, "A An error occurred decoding the data!\n");
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
      fprintf(stderr, "B An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map_data.matrix[i] = mpack_tag_uint_value(&tag);
    } else {
      map_data.matrix[i] = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "C An error occurred decoding the data!\n");
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
      fprintf(stderr, "D An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map_data.spot_types[i] = mpack_tag_uint_value(&tag);
    } else {
      map_data.spot_types[i] = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "E An error occurred decoding the data!\n");
      return;
    }
  }
  mpack_done_array(&reader);

  reset_map();

  matrix_to_rects();
}


void save_map(const char* map_filename)
{
  printf("save %s\n", map_filename);

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
