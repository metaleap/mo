#version 330

uniform vec2 u_resolution;

float plot(vec2 st, float pct) {
  return smoothstep(pct - 0.02, pct, st.y)
          - smoothstep(pct, pct + 0.02, st.y);
}

void main() {
  vec2 st = gl_FragCoord.xy / u_resolution;
  float y = pow(st.x, 5.0);
  vec3 col = vec3(y);
  float pct = plot(st, y);
  col = ((1.0 - pct) * col) + (pct * vec3(0.0, 1.0, 0.0));
  gl_FragColor = vec4(col, 1.0);
}
