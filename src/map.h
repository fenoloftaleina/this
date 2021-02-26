#include "mpack.h"


typedef enum spot_type_status
{
  spot_active,
  spot_inactive
} spot_type_status;


typedef enum spot_type
{
  spot_empty = -1,
  spot_one,
  spot_two,
  spot_three,
  spot_four,
  spot_five
} spot_type;

const int SPRITE_OFFSET = 0;

const int spot_type_n = 5;


typedef struct
{
  int n;
  rect* prev_rects; // prev
  rect* rects;
  spot_type* spot_types;
  spot_type_status spot_type_statuses[spot_type_n];
  spot_type_status prev_spot_type_statuses[spot_type_n];

  int* matrix; // matrix, linear, but 2d

  spot_type* raw_spot_types; // alwaus full size, with -1s in empty ones, for editor and serialization

  float raw_tile_width, raw_tile_height;

  const int matrix_w;
  const int matrix_h;
  const int matrix_size;

  // int offset_x, offsey_y; // for moving between maps potentially

  int* temp_models_list;

  tween_data_t tween_per_type[spot_type_n];
  schedule_data_t reset_schedule;
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
  {0.039f, 0.035f, 0.031f},
  {0.937f, 0.780f, 0.887f},
  {0.631f, 0.853f, 0.846f},
  {0.131f, 0.853f, 0.846f},
  {0.131f, 0.253f, 0.846f},
};

const color death_type_color = (color){0.8f, 0.8f, 0.8f};


float tile_width = 200.0f;
float tile_height = 200.0f;


void init_map()
{
  map_data.prev_rects = (rect*)malloc(map_data.matrix_size * sizeof(rect));
  map_data.rects = (rect*)malloc(map_data.matrix_size * sizeof(rect));
  map_data.spot_types = (spot_type*)malloc(map_data.matrix_size * sizeof(spot_type));
  map_data.matrix = (int*)malloc(map_data.matrix_size * sizeof(int));
  memset(map_data.matrix, -1, map_data.matrix_size * sizeof(int));
  map_data.raw_spot_types = (spot_type*)malloc(map_data.matrix_size * sizeof(spot_type));
  map_data.temp_models_list = (int*)malloc(map_data.matrix_size * sizeof(int));

  for (int i = 0; i < spot_type_n; ++i) {
    map_data.tween_per_type[i].fn = lerp_tween;

    map_data.tween_per_type[i].start_t = 0.0f;
    map_data.tween_per_type[i].end_t = 1.0f;
    map_data.tween_per_type[i].start_v = 0.0f;
    map_data.tween_per_type[i].end_v = 1.0f;
  }

  reset_schedule(&map_data.reset_schedule);
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

    map_data.rects[i].r = map_data.rects[i].g = map_data.rects[i].b =
      map_data.prev_rects[i].r = map_data.prev_rects[i].g = map_data.prev_rects[i].b =
      lerp(death_type_color.r, black_f, map_data.tween_per_type[type].v);

    map_data.temp_models_list[i] = 1;
  }
  add_models(
      &rects_bo, map_data.temp_models_list, map_data.n, 1.0f,
      map_data.rects, map_data.prev_rects, frame_fraction
      );

  for (int i = 0; i < map_data.n; ++i) {
    type = map_data.spot_types[i];

    map_data.prev_rects[i].r = map_data.rects[i].r =
      lerp(
          death_type_color.r,
          type_colors[map_data.spot_types[i]].r,
          map_data.tween_per_type[type].v
          );
    map_data.prev_rects[i].g = map_data.rects[i].g =
      lerp(
          death_type_color.g,
          type_colors[map_data.spot_types[i]].g,
          map_data.tween_per_type[type].v
          );
    map_data.prev_rects[i].b = map_data.rects[i].b =
      lerp(
          death_type_color.b,
          type_colors[map_data.spot_types[i]].b,
          map_data.tween_per_type[type].v
          );

    map_data.temp_models_list[i] = 0;
  }
  add_models(
      &rects_bo, map_data.temp_models_list, map_data.n, 1.0f,
      map_data.rects, map_data.prev_rects, frame_fraction
      );
}


void raw_xy12(const int i, float* x1, float* y1, float* x2, float* y2)
{
  *x1 = -1.0f + (i % map_data.matrix_w * map_data.raw_tile_width);
  *y1 = -1.0f + (i / map_data.matrix_w * map_data.raw_tile_height);
  *x2 = *x1 + map_data.raw_tile_width;
  *y2 = *y1 + map_data.raw_tile_height;
}


void raw_spots_to_matrix()
{
  map_data.raw_tile_width = tile_width;
  map_data.raw_tile_height = tile_height;

  spot_type cur_type;
  float x1, y1, x2, y2;
  int j = 0;
  for(int i = 0; i < map_data.matrix_size; ++i) {
    if (map_data.raw_spot_types[i] != -1) {
      cur_type = map_data.raw_spot_types[i];

      raw_xy12(i, &x1, &y1, &x2, &y2);

      map_data.matrix[i] = j;
      map_data.rects[j] = (rect){
        x1, y1, x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, flat_z, -1.0f, -1.0f
      };

      map_data.prev_rects[j] = map_data.rects[j];
      map_data.spot_types[j] = cur_type;

      ++j;
    }
  }

  map_data.n = j;

  for (int i = 0; i < spot_type_n; ++i) {
    map_data.spot_type_statuses[i] = spot_active;
    map_data.prev_spot_type_statuses[i] = spot_active;
  }
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
  // int cnt = mpack_tag_array_count(&tag);
  for (int i = 0; i < map_data.matrix_size; ++i) {
    tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map_data.raw_spot_types[i] = mpack_tag_uint_value(&tag);
    } else {
      map_data.raw_spot_types[i] = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
  }
  mpack_done_array(&reader);


  raw_spots_to_matrix();
}


void save_map(const char* map_filename)
{
  char file_path[255];
  sprintf(file_path, "%ssrc/maps/%s", main_dir, map_filename);
  mpack_writer_t writer;
  mpack_writer_init_filename(&writer, file_path);
  mpack_start_array(&writer, map_data.matrix_size);
  for (int i = 0; i < map_data.matrix_size; ++i) {
    mpack_write_int(&writer, map_data.raw_spot_types[i]);
  }
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    fprintf(stderr, "An error occurred encoding the data!\n");
    return;
  }
}


void set_raw_spot(const int x, const int y, const spot_type t)
{
  map_data.raw_spot_types[y * map_data.matrix_w + x] = t;

  raw_spots_to_matrix();
}


spot_type get_raw_spot(const int x, const int y)
{
  return map_data.raw_spot_types[y * map_data.matrix_w + x];
}
