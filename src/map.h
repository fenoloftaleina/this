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
  spot_blue,
  spot_red_dead,
  spot_green_dead,
  spot_blue_dead
} spot_type;

const int spot_type_n = 3;


typedef struct map_data
{
  buffer_object bo;

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
} map_data;


typedef struct color
{
  float r, g, b;
} color;

color type_colors[] = {
  {0.5f, 0.35f, 0.47f},
  {0.4f, 0.6f, 0.4f},
  {0.3f, 0.6f, 0.7f}
};

color dead_type_colors[] = {
  {1.0f, 0.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
  {0.0f, 0.0f, 1.0f}
};


float tile_width = 200.0f;
float tile_height = 200.0f;


void init_map(map_data* map)
{
  init_rects_buffer_object(&map->bo, map->matrix_size);
  set_buffer_counts(&map->bo, 0, 0);

  map->prev_rects = (rect*)malloc(map->matrix_size * sizeof(rect));
  map->rects = (rect*)malloc(map->matrix_size * sizeof(rect));
  map->spot_types = (spot_type*)malloc(map->matrix_size * sizeof(spot_type));
  map->spot_statuses = (spot_status*)malloc(map->matrix_size * sizeof(spot_status));
  map->matrix = (int*)malloc(map->matrix_size * sizeof(int));
  memset(map->matrix, -1, map->matrix_size * sizeof(int));
  map->raw_spot_types = (spot_type*)malloc(map->matrix_size * sizeof(spot_type));
}


void draw_map(map_data* map, const float frame_fraction)
{
  draw_rects(&map->bo, map->prev_rects, map->rects, map->n, frame_fraction);
}


void raw_spots_to_matrix(map_data* map)
{
  map->raw_tile_width = tile_width / sapp_width();
  map->raw_tile_height = tile_height / sapp_height();

  spot_type cur_type;
  float x, y;
  int j = 0;
  for(int i = 0; i < map->matrix_size; ++i) {
    if (map->raw_spot_types[i] != -1) {
      cur_type = map->raw_spot_types[i];

      x = -1.0f + (i % map->matrix_w * map->raw_tile_width);
      y = -1.0f + (i / map->matrix_w * map->raw_tile_height);

      map->matrix[i] = j;
      map->rects[j] = (rect){
        x,
        y,
        x + map->raw_tile_width,
        y + map->raw_tile_height,
        type_colors[cur_type].r,
        type_colors[cur_type].g,
        type_colors[cur_type].b,
        1.0f
      };
      map->prev_rects[j] = map->rects[j];
      map->spot_types[j] = cur_type;
      map->spot_statuses[j] = spot_active;

      ++j;
    }
  }

  map->n = j;

  set_buffer_counts(
      &map->bo,
      rects_write_vertices_simple(map->rects, &map->bo, map->n),
      rects_write_indices(&map->bo, map->n)
      );
  // update_buffer_vertices(&map->bo);
  update_buffer_indices(&map->bo);
}


void load_map(map_data* map, const char* map_filename)
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
  for (int i = 0; i < map->matrix_size; ++i) {
    tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      map->raw_spot_types[i] = mpack_tag_uint_value(&tag);
    } else {
      map->raw_spot_types[i] = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
  }
  mpack_done_array(&reader);


  raw_spots_to_matrix(map);
}


void save_map(map_data* map, const char* map_filename)
{
  char file_path[255];
  sprintf(file_path, "../../../main/src/maps/%s", map_filename);
  mpack_writer_t writer;
  mpack_writer_init_filename(&writer, file_path);
  mpack_start_array(&writer, map->matrix_size);
  for (int i = 0; i < map->matrix_size; ++i) {
    mpack_write_int(&writer, map->raw_spot_types[i]);
  }
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    fprintf(stderr, "An error occurred encoding the data!\n");
    return;
  }
}


void set_raw_spot(map_data* map, const int x, const int y, const spot_type t)
{
  map->raw_spot_types[y * map->matrix_w + x] = t;

  raw_spots_to_matrix(map);
}


spot_type get_raw_spot(map_data* map, const int x, const int y)
{
  return map->raw_spot_types[y * map->matrix_w + x];
}
