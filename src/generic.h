typedef struct
{
  buffer_object bo;
} generic_data;


void init_generic
(generic_data* generic, const int vertices_count, const int indices_count)
{
  init_buffer_object(&generic->bo, vertices_count, indices_count);
}


void draw_generic(const generic_data* generic)
{
  draw_buffer_object(&generic->bo);
}
