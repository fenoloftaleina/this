
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
  int spots[PATHS_N];

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


int ii_to_path(int ii)
{
  for (int i = 0; i < paths_data.n; ++i) {
    if (paths_data.spots[i] == ii) {
      return i;
    }
  }

  return -1;
}
