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


// 2D Noise based on Morgan McGuire @morgan3d https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = rand2(i);
    float b = rand2(i + vec2(1.0, 0.0));
    float c = rand2(i + vec2(0.0, 1.0));
    float d = rand2(i + vec2(1.0, 1.0));

    // Smooth Interpolation
    vec2 u = smoothstep(0.0, 1.0, f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            ((c - a) * u.y * (1.0 - u.x)) +
            ((d - b) * u.x * u.y);
}


void main() {
  vec2 st = gl_FragCoord.xy / u_resolution.xy;
  // Scale the coordinate system to see some noise in action
  vec2 pos = vec2(st * 11.0);
  // Use the noise function
  float n = noise(pos);

  out_FragColor = vec4(vec3(n), 1.0);
}
