#version 450

uniform vec2 u_resolution;

out vec4 out_FragColor;


vec2 rand2(vec2 st) {
  st = vec2(dot(st, vec2(127.1, 311.7)),
            dot(st, vec2(269.5, 183.3)));
  return -1.0 + (2.0 * fract(sin(st) * 43758.5453123));
}


float noiseIq(in vec2 st) {
  vec2 i = floor(st);
  vec2 f = fract(st);
  vec2 u = smoothstep(0.0, 1.0, f);
  return mix( mix( dot( rand2(i + vec2(0.0,0.0) ),
                         f - vec2(0.0,0.0) ),
                   dot( rand2(i + vec2(1.0,0.0) ),
                         f - vec2(1.0,0.0) ), u.x),
              mix( dot( rand2(i + vec2(0.0,1.0) ),
                         f - vec2(0.0,1.0) ),
                   dot( rand2(i + vec2(1.0,1.0) ),
                         f - vec2(1.0,1.0) ), u.x), u.y);
}


void main() {
  vec2 st = gl_FragCoord.xy / u_resolution.xy;
  st.x *= u_resolution.x / u_resolution.y;
  vec3 color = vec3(0.0);
  vec2 pos = vec2(st * 11.0);
  color = vec3(noiseIq(pos) * 0.5 + 0.5);
  out_FragColor = vec4(color, 1.0);
}
