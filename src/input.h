typedef enum {
  IN_UP, IN_DOWN, IN_LEFT, IN_RIGHT, IN_NONE
} in_type;

typedef struct
{
  in_type h;
  in_type v;
} input_data;
