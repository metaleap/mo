#version 450

uniform vec2 u_resolution;

out vec4 out_FragColor;


float rand2(vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}


float hash(vec2 p) {
    p  = 50.0 * fract(p * 0.3183099 + vec2(0.71, 0.113));
    return -1.0 + (2.0 * fract(p.x * p.y * (p.x + p.y)));
}


float noiseIq(in vec2 st) {
  vec2 i = floor(st);
  vec2 f = fract(st);
  vec2 u = smoothstep(0.0, 1.0, f);
  return mix( mix(hash(i + vec2(0.0,0.0)),
                  hash(i + vec2(1.0,0.0)), u.x),
              mix(hash(i + vec2(0.0,1.0)),
                  hash(i + vec2(1.0,1.0)), u.x), u.y);
}


void main() {
  vec2 st = gl_FragCoord.xy / u_resolution.xy;
  st.x *= u_resolution.x / u_resolution.y;
  vec3 color = vec3(0.0);
  vec2 pos = vec2(st * 11.0);
  color = vec3(noiseIq(pos) * 0.5 + 0.5);
  out_FragColor = vec4(color, 1.0);
}
