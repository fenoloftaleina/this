#include "mpack.h"


typedef enum spot_status
{
  spot_active,
  spot_inactive
} spot_status;


typedef enum spot_type
{
  spot_empty = -1,
  spot_red,
  spot_green,
  spot_blue
} spot_type;

const int SPRITE_OFFSET = 2;

const int spot_type_n = 3;


typedef struct
{
  int n;
  rect* prev_rects; // prev
  rect* rects;
  spot_type* spot_types;
  spot_status* spot_statuses;

  int* matrix; // matrix, linear, but 2d

  spot_type* raw_spot_types; // alwaus full size, with -1s in empty ones, for editor and serialization

  float raw_tile_width, raw_tile_height;

  const int matrix_w;
  const int matrix_h;
  const int matrix_size;

  // int offset_x, offsey_y; // for moving between maps potentially
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


const color death_tint_color = (color){0.3f, 0.3f, 0.3f};


float tile_width = 200.0f;
float tile_height = 200.0f;


void init_map()
{
  map_data.prev_rects = (rect*)malloc(map_data.matrix_size * sizeof(rect));
  map_data.rects = (rect*)malloc(map_data.matrix_size * sizeof(rect));
  map_data.spot_types = (spot_type*)malloc(map_data.matrix_size * sizeof(spot_type));
  map_data.spot_statuses = (spot_status*)malloc(map_data.matrix_size * sizeof(spot_status));
  map_data.matrix = (int*)malloc(map_data.matrix_size * sizeof(int));
  memset(map_data.matrix, -1, map_data.matrix_size * sizeof(int));
  map_data.raw_spot_types = (spot_type*)malloc(map_data.matrix_size * sizeof(spot_type));
}


void draw_map(const float frame_fraction)
{
  add_rects(&sprites_bo, map_data.rects, map_data.prev_rects, map_data.n, frame_fraction);
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
        x1,
        y1,
        x2,
        y2,
        1.0f,
        1.0f,
        1.0f,
        1.0f
      };
      set_sprite(&map_data.rects[j], &texture, cur_type + SPRITE_OFFSET);

      map_data.prev_rects[j] = map_data.rects[j];
      map_data.spot_types[j] = cur_type;
      map_data.spot_statuses[j] = spot_active;

      ++j;
    }
  }

  map_data.n = j;
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
  sprintf(file_path, "../../../main/src/maps/%s", map_filename);
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
