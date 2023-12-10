#version 450

uniform vec2 u_resolution;

out vec4 out_FragColor;


float rand1(float n, float factor) {
  return fract(sin(n) * factor);
}

float rand2(vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float noise1(float n) {
  float i = floor(n);  // integer
  float j = fract(n);  // fraction
  return mix(rand1(i, 1.0), rand1(i + 1.0, 1.0), smoothstep(0.0, 1.0, j));
}


void main() {
  vec2 st = (8.5 * (gl_FragCoord.xy / u_resolution)) - (8.5 / 2.0);
  float x = st.x;
  float y = noise1(x);
  vec3 col = vec3(0.2); // default dark backdrop

  // grid line if st nearly integer
  vec2 f = abs(fract(st));
  if (((f.x <= 0.01) || (f.x >= 0.09)) && ((f.y <= 0.01) || (f.y >= 0.09))) { col = vec3(0.1); }
  // backdrop: lighter if st -1..1
  if ((st.x >= -1.0) && (st.x <= 1.0) && (st.y >= -1.0) && (st.y <= 1.0)) { col = vec3(0.4); }
  // plot
  if ((st.y >= (y - 0.044)) && (st.y <= (y + 0.044))) { col = vec3(0.8, 0.6, 0.2); }

  out_FragColor = vec4(col, 1.0);
}
