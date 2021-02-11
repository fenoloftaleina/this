typedef struct
{
  float x1, y1, x2, y2;
} mapping_data;

static const int texture_size = 1024;

typedef struct
{
  mapping_data* mappings;
  int mappings_n;

  unsigned char* pixels;
  sg_image image_id;
} texture_data;


// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"


void init_texture(texture_data* texture, const char** paths, const int paths_n)
{
  int width, height, nrChannels;
  unsigned char* image;
  texture->pixels = (unsigned char*)malloc(texture_size * texture_size * 4 * sizeof(unsigned char));

  stbi_set_flip_vertically_on_load(true);

  texture->mappings_n = paths_n;
  texture->mappings = (mapping_data*)malloc(texture->mappings_n * sizeof(mapping_data));

  char temp_path[255];

  int i = 0, j = 0, cur_line_max_height = 0;

  for(int path_i = 0; path_i < paths_n; ++path_i) {
    strcpy(temp_path, main_dir);
    strcat(temp_path, paths[path_i]);

    image = stbi_load(temp_path, &width, &height, &nrChannels, 0);
    // printf("%d %d %d\n", width, height, nrChannels);

    if (i + width + 1 >= texture_size) {
      i = 0;
      j = cur_line_max_height + 1;

      cur_line_max_height = 0;
    }

    for(int m = 0; m < height; ++m) {
      memcpy(texture->pixels + i * 4 + j * texture_size * 4 + m * texture_size * 4, image + m * width * 4, width * 4);
    }

    texture->mappings[path_i] = (mapping_data){
      i / (float)texture_size,
      j / (float)texture_size,
      (i + width) / (float)texture_size,
      (j + height) / (float)texture_size
    };

    i += width + 1;

    if (height > cur_line_max_height) {
      cur_line_max_height = height;
    }

    stbi_image_free(image);
  }

  // stbi_write_png("/Users/ja/Downloads/asdf.png", 1024, 1024, 4, texture->pixels, 0);
}
