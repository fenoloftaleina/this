#include "mpack.h"


typedef enum spot_status
{
  spot_alive,
  spot_dead
} spot_status;


typedef enum spot_type
{
  spot_empty = -1,
  spot_red,
  spot_green,
  spot_blue
} spot_type;

const int spot_type_n = 3;


typedef struct map_data
{
  buffer_object bo;

  int n;
  rect* prs; // prev
  rect* rs;
  spot_type* ts;
  spot_status* ss;

  int* m; // matrix, linear, but 2d

  spot_type* spots;

  bool changed;

  // int offset_x, offsey_y; // for moving between maps potentially
} map_data;


typedef struct color
{
  float r, g, b;
} color;

color type_colors[] = {
  {0.5, 0.35, 0.47},
  {0.4, 0.6, 0.4},
  {0.3, 0.6, 0.7}
};


float tile_width = 200.0f;
float tile_height = 200.0f;

const int matrix_w = 20;
const int matrix_h = 12;
const int matrix_size = matrix_w * matrix_h;


void init_map(map_data* md)
{
  init_buffer_object(&md->bo, matrix_size * vertices_per_rect, matrix_size * indices_per_rect);
  set_buffer_counts(&md->bo, 0, 0);

  md->rs = (rect*)malloc(matrix_size * sizeof(rect));
  md->ts = (spot_type*)malloc(matrix_size * sizeof(spot_type));
  md->ss = (spot_status*)malloc(matrix_size * sizeof(spot_status));
  md->m = (int*)malloc(matrix_size * sizeof(int));
  memset(md->m, -1, matrix_size * sizeof(int));
  md->spots = (spot_type*)malloc(matrix_size * sizeof(spot_type));

  md->changed = false;
}


void draw_map(map_data* md, const float frame_fraction)
{
  if (md->changed) {
    rects_write_vertices(md->prs, md->rs, &md->bo, md->n, frame_fraction),
    update_buffer_vertices(&md->bo);
  }

  draw_buffer_object(&md->bo);
}


void spots_to_matrix(map_data* md)
{
  float tw = tile_width / sapp_width();
  float th = tile_height / sapp_height();


  spot_type cur_type;
  float x, y;
  int j = 0;
  for(int i = 0; i < matrix_size; ++i) {
    if (md->spots[i] != -1) {
      cur_type = md->spots[i];

      x = -1.0f + (i % matrix_w * tw);
      y = -1.0f + (i / matrix_w * th);

      md->m[i] = j;
      md->rs[j] = (rect){
        x,
        y,
        x + tw,
        y + th,
        type_colors[cur_type].r,
        type_colors[cur_type].g,
        type_colors[cur_type].b,
        1.0f
      };
      md->ts[j] = cur_type;
      md->ss[j] = spot_alive;

      ++j;
    }
  }

  md->n = j;

  set_buffer_counts(
      &md->bo,
      rects_write_vertices_simple(md->rs, &md->bo, md->n),
      rects_write_indices(&md->bo, md->n)
      );
  update_buffer_vertices(&md->bo);
  update_buffer_indices(&md->bo);

  md->changed = false;
}


void load_map(map_data* md, const char* map_filename)
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
  for (int i = 0; i < matrix_size; ++i) {
    tag = mpack_read_tag(&reader);
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
    if (mpack_tag_type(&tag) == mpack_type_uint) {
      md->spots[i] = mpack_tag_uint_value(&tag);
    } else {
      md->spots[i] = mpack_tag_int_value(&tag);
    }
    if (mpack_reader_error(&reader) != mpack_ok) {
      fprintf(stderr, "An error occurred decoding the data!\n");
      return;
    }
  }
  mpack_done_array(&reader);


  spots_to_matrix(md);
}


void save_map(map_data* md, const char* map_filename)
{
  char file_path[255];
  sprintf(file_path, "../../../main/src/maps/%s", map_filename);
  mpack_writer_t writer;
  mpack_writer_init_filename(&writer, file_path);
  mpack_start_array(&writer, matrix_size);
  for (int i = 0; i < matrix_size; ++i) {
    mpack_write_int(&writer, md->spots[i]);
  }
  mpack_finish_array(&writer);
  if (mpack_writer_destroy(&writer) != mpack_ok) {
    fprintf(stderr, "An error occurred encoding the data!\n");
    return;
  }
}


void set_spot(map_data* md, const int x, const int y, const spot_type t)
{
  md->spots[y * matrix_w + x] = t;

  spots_to_matrix(md);
}


spot_type get_spot(map_data* md, const int x, const int y)
{
  return md->spots[y * matrix_w + x];
}
