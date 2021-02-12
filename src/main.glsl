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

void main() {
  if (uv.x >= 0.0) {
    frag_color = texture(tex, uv) * color;

    if (frag_color.a < 0.1) {
      discard;
    }
  } else {
    frag_color = color;
  }

  float d = 0.02;

  if (uv.x > d && uv.x < 1 - d && uv.y > d && uv.y < 1 - d) {
    frag_color = vec4(1);
  } else {
    frag_color = vec4(vec3(0.7), 1.0);
  }
}
@end


@program uv_frag vs_uv_frag fs_uv_frag
