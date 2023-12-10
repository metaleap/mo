#version 450

#define PI 3.14159265359

uniform vec2 u_resolution;

out vec4 out_FragColor;


float rand2(vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}


// 2D Noise based on Morgan McGuire @morgan3d https://www.shadertoy.com/view/4dS3Wd
float noiseMmg(in vec2 st) {
  vec2 i = floor(st);
  vec2 f = fract(st);
  // Smooth Interpolation
  vec2 u = smoothstep(0.0, 1.0, f);
  // Four corners in 2D of a tile
  float a = rand2(i);
  float b = rand2(i + vec2(1.0, 0.0));
  float c = rand2(i + vec2(0.0, 1.0));
  float d = rand2(i + vec2(1.0, 1.0));
  // Mix 4 corners percentages
  return mix(a, b, u.x) +
          ((c - a) * u.y * (1.0 - u.x)) +
          ((d - b) * u.x * u.y);
}


#define OCTAVES 6
float fbm (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noiseMmg(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}


void main() {
  vec2 st = gl_FragCoord.xy / u_resolution.xy;
  st.x *= u_resolution.x / u_resolution.y;
  st *= 3.0;

  out_FragColor = vec4(vec3(fbm(st)), 1.0);
}
