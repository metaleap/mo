#version 330

uniform vec2 u_resolution;

float plot(vec2 st) {
  return 1.0 - smoothstep(0.0, 0.02, abs(st.y - st.x));
}

void main() {
  vec2 st = gl_FragCoord.xy / u_resolution;
  float y = st.x;
  float pct = plot(st);
  vec3 col = ((1.0 - pct) * vec3(y)) + (pct * vec3(0.0, 1.0, 0.0));
  gl_FragColor = vec4(col, 1.0);
}
