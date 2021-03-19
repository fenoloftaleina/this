
static const int PATHS_N = 100;
static const int PATHS_M = 20;

typedef struct
{
  int i;
  int j;
} ij_t;

typedef struct
{
  int lengths[PATHS_N];
  int directions[PATHS_N];
  int step[PATHS_N];
  bool looped[PATHS_N];
  int jjs[PATHS_N];

  ij_t positions[PATHS_N * PATHS_M];

  int n;
} paths_data_t;


paths_data_t paths_data;


void reset_paths()
{
  paths_data.n = 0;
}


void init_paths()
{
  reset_paths();
}


void add_to_path(int path, int i, int j)
{
  int pos_id = path * PATHS_M + paths_data.lengths[path];
  paths_data.positions[pos_id].i = i;
  paths_data.positions[pos_id].j = j;

  paths_data.lengths[path] += 1;
}


int restart_path(int jj, int i, int j)
{
  int path = -1;

  for (int i = 0; i < paths_data.n; ++i) {
    if (paths_data.jjs[i] == jj) {
      path = i;
    }
  }

  if (path == -1) {
    path = paths_data.n;
    paths_data.n += 1;

    paths_data.jjs[path] = jj;
  }

  paths_data.lengths[path] = 0;
  paths_data.directions[path] = 1;
  paths_data.step[path] = 0;
  paths_data.looped[path] = false;

  add_to_path(path, i, j);

  return path;
}


int jj_to_path(int jj)
{
  for (int i = 0; i < paths_data.n; ++i) {
    if (paths_data.jjs[i] == jj) {
      return i;
    }
  }

  return -1;
}


void draw_paths()
{
  lines_data.thickness = 4;
  pos_t lines_positions[PATHS_M];

  int k = 0;
  for (int i = 0; i < paths_data.n; ++i) {
    if (paths_data.lengths[i] < 2) {
      continue;
    }

    for (int j = 0; j < paths_data.lengths[i]; ++j) {
      int pos_id = i * PATHS_M + j;
      lines_positions[j] = (pos_t){
        paths_data.positions[pos_id].i * map_data.raw_tile_width +
          map_data.raw_tile_width * 0.5f,
        paths_data.positions[pos_id].j * map_data.raw_tile_height +
          map_data.raw_tile_height * 0.5f
      };
    }

    add_lines(
        &lines_bo, lines_positions, paths_data.lengths[i],
        &(col_t){0.7f, 0.1f, 0.4f, 1.0f}, flat_z, false
        );
  }
}
