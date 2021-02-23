@ctype mat4 hmm_mat4


@vs vs
uniform vs_params {
    mat4 mvp;
};

in vec4 position;
in vec4 color0;
in vec2 texcoord0;

out vec4 color;
out vec2 uv;

void main() {
  gl_Position = mvp * position;
  color = color0;
  uv = texcoord0 * 1.0;
}
@end


@fs fs
uniform sampler2D tex;

in vec4 color;
in vec2 uv;

out vec4 frag_color;

void main() {
  if (uv.x >= 0.0) {
    frag_color = texture(tex, uv) * color;

    if (frag_color.a < 0.1) {
      discard;
    }
  } else {
    frag_color = color;
  }
}
@end


@program main vs fs




@vs vs_uv_frag
uniform vs_params {
    mat4 mvp;
};

in vec4 position;
in vec4 color0;
in vec2 texcoord0;

out vec4 color;
out vec2 uv;

void main() {
  gl_Position = mvp * position;
  color = color0;
  uv = texcoord0 * 1.0;
}
@end


@fs fs_uv_frag
uniform sampler2D tex;

in vec4 color;
in vec2 uv;

out vec4 frag_color;

bool eps(float a, float b, float epsilon)
{
  return abs(a - b) < epsilon;
}

void main() {
  float x = uv.x;
  float y = uv.y;

  if (x >= 0.0) {
    frag_color = texture(tex, uv) * color;

    if (frag_color.a < 0.1) {
      discard;
    }
  } else {
    frag_color = color;
  }

  float d = 0.02;

  if (x < d || x > 1 - d || y < d || y > 1 - d) {
    frag_color = vec4(vec3(0.7), 1.0);
  } else {
    frag_color = vec4(1);
  }
}
@end


@program uv_frag vs_uv_frag fs_uv_frag




@vs vs_death
uniform vs_params {
    mat4 mvp;
};

in vec4 position;
in vec4 color0;
in vec2 texcoord0;

out vec4 color;
out vec2 uv;

void main() {
  gl_Position = mvp * position;
  color = color0;
  uv = texcoord0 * 1.0;
}
@end


@fs fs_death
uniform sampler2D tex;

in vec4 color;
in vec2 uv;

out vec4 frag_color;

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float hash(float n)
{
  return fract(sin(n)*43758.5453);
}

float noise(vec3 x)
{
  // The noise function returns a value in the range -1.0f -> 1.0f

  vec3 p = floor(x);
  vec3 f = fract(x);

  f       = f*f*(3.0-2.0*f);
  float n = p.x + p.y*57.0 + 113.0*p.z;

  return mix(mix(mix( hash(n+0.0), hash(n+1.0),f.x),
                 mix( hash(n+57.0), hash(n+58.0),f.x),f.y),
             mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                 mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

bool eps(float a, float b, float epsilon)
{
  return abs(a - b) < epsilon;
}

float expStep( float x, float k, float n )
{
    return exp( -k*pow(x,n) );
}

float pcurve(float x, float a, float b)
{
    const float k = pow(a+b,a+b) / (pow(a,a)*pow(b,b));
    return k * pow( x, a ) * pow( 1.0-x, b );
}

#define PI 3.14159265359

void main() {
  frag_color = texture(tex, uv);

  vec2 st = gl_FragCoord.xy / vec2(1000.0f);
  float division = 120.0f;
  st = ceil(st * division) / (division * 0.5f);

  vec3 noisy_cells = vec3(noise(vec3(st * 100.0f, 0.0f)));

  if (noisy_cells.x > pcurve(color.a, 0.5f, 0.7f) * 0.4f) {
    discard;
  }

  float gray_tween = expStep(color.a * 1.5f, 1.0f, 3.0f);
  float alpha_tween = expStep(color.a * 2.5f, 1.0f, 3.0f);
  vec3 col = color.rgb;
  col.g = col.g * gray_tween + col.r * (1.0f - gray_tween);
  col.b = col.b * gray_tween + col.r * (1.0f - gray_tween);

  frag_color = vec4((1.0f - noisy_cells) * col, alpha_tween * 0.6f + 0.4f);
}
@end


@program death vs_death fs_death
