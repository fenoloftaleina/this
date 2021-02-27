#include "time.h"

static int cur_map_i;
static char cur_map_name[255];
static char* map_list[255];
static int map_list_n;

void load_map_list()
{
  map_list_n = 0;
  char file_path[255];
  sprintf(file_path, "%ssrc/maps/maps_list", main_dir);
  FILE* fp = fopen(file_path, "r");
  const int buffer_length = 255;
  char buffer[buffer_length];
  while (fgets(buffer, buffer_length, fp)) {
    buffer[strcspn(buffer, "\r\n")] = 0;
    map_list[map_list_n] = (char*)malloc(255 * sizeof(char));
    strcpy(map_list[map_list_n], buffer);
    map_list_n += 1;
  }
  fclose(fp);
}

void save_map_list()
{
  char file_path[255];
  sprintf(file_path, "%ssrc/maps/maps_list", main_dir);
  FILE* fp = fopen(file_path, "w");
  for (int i = 0; i < map_list_n; ++i) {
    fprintf(fp, "%s\n", map_list[i]);
  }
  fclose(fp);
}

void run_map(const int map_i)
{
  cur_map_i = map_i;
  strcpy(cur_map_name, map_list[cur_map_i]);
  load_map(cur_map_name);
  reload_logic();
  reset_player(map_data.player_start_x, map_data.player_start_y);
}


void duplicate_current_map()
{
  map_list[map_list_n] = (char*)malloc(255 * sizeof(char));

  for (int i = map_list_n; i > cur_map_i + 1; i--) {
    printf("%d\n", i);
    map_list[i] = map_list[i - 1];
  }
  sprintf(map_list[cur_map_i + 1], "%s-%ld", map_list[cur_map_i], time(0));

  map_list_n += 1;
  cur_map_i += 1;

  save_map_list();

  run_map(cur_map_i);
}


void reload_current_map()
{
  run_map(cur_map_i);
}


void save_current_map()
{
  save_map(cur_map_name);
}
