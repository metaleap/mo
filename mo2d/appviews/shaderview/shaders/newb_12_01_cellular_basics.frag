#version 450

uniform vec2 u_resolution;

out vec4 out_FragColor;


void main() {
  vec2 st = gl_FragCoord.xy / u_resolution.xy;
  st.x *= u_resolution.x / u_resolution.y;

  vec2 point[4];
  point[0] = vec2(0.83, 0.75);
  point[1] = vec2(0.60, 0.07);
  point[2] = vec2(0.28, 0.64);
  point[3] =  vec2(0.31, 0.26);

  float dist = 1.0;
  for (int i = 0; i < 4; i++)
    dist = min(dist, distance(st, point[i]));

  out_FragColor = vec4(vec3(dist), 1.0);
}
