void add_quad(
    buffer_object* bo,
    vertex_t* new_vertices,
    index_t* new_indices)
{
  put_in_buffer(bo, new_vertices, 4, new_indices, 6);
}


bool potential_or_real_intersect
(const float x1, const float y1, const float x2, const float y2,
 const float x3, const float y3, const float x4, const float y4,
 float* x, float* y)
{
  float t, u;

  if ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4) == 0) {
    return false;
  } else {
    t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) /
      ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));
    u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) /
      ((x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4));

    *x = x1 + t * (x2 - x1);
    *y = y1 + t * (y2 - y1);

    return true;
  }
}


index_t quad_indices[] = {0, 1, 2, 0, 2, 3};


void line(
  buffer_object* bo,
  const hmm_vec2* line_points,
  const int line_points_count,
  const hmm_vec4* colors,
  const float width)
{
  float half_width = width * 0.5f;

  hmm_vec2 XYs[4 * line_points_count];
  int XYs_count = 0;

  for (int i = 1; i < line_points_count; ++i) {
    hmm_vec2 cur_normalized_vec = HMM_NormalizeVec2(
        HMM_SubtractVec2(
          line_points[i - 1],
          line_points[i]));

    hmm_vec2 perpendicular =
      HMM_MultiplyVec2f((hmm_vec2){
          cur_normalized_vec.Y * -1.0f,
          cur_normalized_vec.X
          }, half_width);

    int j = (i - 1) * 4;

    XYs[j + 0] = HMM_SubtractVec2(line_points[i - 1], perpendicular);
    XYs[j + 1] = HMM_AddVec2(line_points[i - 1], perpendicular);

    XYs[j + 2] = HMM_SubtractVec2(line_points[i], perpendicular);
    XYs[j + 3] = HMM_AddVec2(line_points[i], perpendicular);

    if (i > 1) {
      float x, y;

      if (potential_or_real_intersect(
            XYs[j - 4 + 0].X, XYs[j - 4 + 0].Y, XYs[j - 4 + 2].X, XYs[j - 4 + 2].Y,
            XYs[j + 0].X, XYs[j + 0].Y, XYs[j + 2].X, XYs[j + 2].Y,
            &x, &y)) {
        XYs[j - 4 + 2].X = x;
        XYs[j - 4 + 2].Y = y;
        XYs[j + 0].X = x;
        XYs[j + 0].Y = y;
      }

      if (potential_or_real_intersect(
            XYs[j - 4 + 1].X, XYs[j - 4 + 1].Y, XYs[j - 4 + 3].X, XYs[j - 4 + 3].Y,
            XYs[j + 1].X, XYs[j + 1].Y, XYs[j + 3].X, XYs[j + 3].Y,
            &x, &y)) {
        XYs[j - 4 + 3].X = x;
        XYs[j - 4 + 3].Y = y;
        XYs[j + 1].X = x;
        XYs[j + 1].Y = y;
      }
    }

    XYs_count += 4;

  }

  for (int i = 0; i < XYs_count / 4; ++i) {
    add_quad(
        bo,
        (vertex_t[]){
        XYs[i * 4 + 1].X, XYs[i * 4 + 1].Y, 1.0f, colors[i].X, colors[i].Y, colors[i].Z, colors[i].W, 0.0f, 0.0f,
        XYs[i * 4 + 3].X, XYs[i * 4 + 3].Y, 1.0f, colors[i + 1].X, colors[i + 1].Y, colors[i + 1].Z, colors[i + 1].W, 0.0f, 0.0f,
        XYs[i * 4 + 2].X, XYs[i * 4 + 2].Y, 1.0f, colors[i + 1].X, colors[i + 1].Y, colors[i + 1].Z, colors[i + 1].W, 0.0f, 0.0f,
        XYs[i * 4 + 0].X, XYs[i * 4 + 0].Y, 1.0f, colors[i].X, colors[i].Y, colors[i].Z, colors[i].W, 0.0f, 0.0f
        },
        quad_indices);
  }
}


static const int n = 100;
static float rs[n];


void init_ments()
{
  for (int i = 0; i < n; ++i) {
    rs[i] = rand() / (float)RAND_MAX;
  }
}


#include <time.h>
#define PI 3.14159265

float up_sin180(const float s)
{
  float a = s * 1.15f + 1.0f;

  if (a < 0.0f) {
    a = 0.0f;
  } else if (a > 2.0f) {
    a = 2.0f;
  }

  return a * 90.0f;
}

void draw_ments(const float t)
{
  srand(time(0));

  hmm_vec2 line_points[2];
  hmm_vec4 line_colors[2];
  int line_points_count = 2;
  float line_width = 2.0f;

  float size;

  reset_buffer_counts(&generic.bo);

  float ang, ang2;

  for (int i = 0; i < n; ++i) {
    float x1, y1, x2, y2, add_x, add_y;
    x1 = 1100.0f;
    y1 = 50.0f;
    size = 500.0f;

    float ii = i * 0.01f;

    add_x = (1.0f + ii) * size;
    add_y = 0.0f * size;

    // x2 = x1 + add_x;
    // y2 = y1 + add_y;

    float ang2_offset = 0.1f;

    ang = (180.0f - up_sin180(sin(t + rs[i]))) * PI / 180.0f;
    ang2 = (180.0f - up_sin180(sin(t + rs[i] + ang2_offset))) * PI / 180.0f;
    // ang = 100.0f;

    line_points[0].X = x1 + add_x * cos(ang) - add_y * sin(ang);
    line_points[0].Y = y1 + add_x * sin(ang) + add_y * cos(ang);
    line_points[1].X = x1 + add_x * cos(ang2) - add_y * sin(ang2);
    line_points[1].Y = y1 + add_x * sin(ang2) + add_y * cos(ang2);

    line_colors[0] = (hmm_vec4){0, 0, 0, 1};
    line_colors[1] = line_colors[0];

    line(
        &generic.bo,
        line_points,
        line_points_count,
        line_colors,
        line_width
        );
  }

  update_buffer_vertices(&generic.bo);
  update_buffer_indices(&generic.bo);


  // reset_buffer_counts(&generic2.bo);
  //
  // for (int i = 100; i < 200; ++i ) {
  //   for (int j = 0; j < 200; ++j ) {
  //     float x, y;
  //     x = i + 5;
  //     y = j + 5;
  //     line_points[0].X = x * size;
  //     line_points[0].Y = y * size;
  //     // line_points[1].X = (i + 0.7) * size + sin(t) * 20.0f;
  //     // line_points[1].Y = (j + 0.7) * size;
  //     line_points[1].X = (x + 0.7) * size;
  //     line_points[1].Y = (y + 0.7) * size;
  //
  //     line(
  //         &generic2.bo,
  //         line_points,
  //         line_points_count,
  //         line_width
  //         );
  //   }
  // }
  //
  // update_buffer_vertices(&generic2.bo);
  // update_buffer_indices(&generic2.bo);


  // printf("%d %d %d\n", generic.bo.vertices_count, generic.bo.vertices_count / vertex_elements_count, vertex_elements_count);
  // printf("%d\n", generic.bo.indices_count);
}
