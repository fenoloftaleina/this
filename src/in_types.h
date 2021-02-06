typedef enum {
  IN_UP, IN_DOWN, IN_LEFT, IN_RIGHT, IN_NONE
} in_type;

typedef struct
{
  in_type h;
  in_type v;
  bool editor;
} input_data;

input_data in = {IN_NONE, IN_NONE};
