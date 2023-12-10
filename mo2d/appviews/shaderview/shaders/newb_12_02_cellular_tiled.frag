#version 450

uniform vec2 u_resolution;

out vec4 out_FragColor;


vec2 random2(vec2 p) {
    return fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5453);
}


void main() {
  vec2 st = gl_FragCoord.xy / u_resolution.xy;
  st.x *= u_resolution.x / u_resolution.y;
  st *= 11.0;

  // Tile the space
  vec2 i_st = floor(st);
  vec2 f_st = fract(st);

  float dist = 1.0;
  for (int y= -1; y <= 1; y++) {
    for (int x= -1; x <= 1; x++) {
      // Neighbor place in the grid
      vec2 neighbor = vec2(float(x), float(y));
      // Random position from current + neighbor place in the grid
      vec2 point = random2(i_st + neighbor);
      // Animate the point
      float u_time = 0.5; // would be shader uniform changing per frame
      point = 0.5 + 0.5*sin((u_time*0.5) + 6.2831*point);
      // Vector between the pixel and the point
      vec2 diff = (neighbor + point) - f_st;
      // Keep the closer distance
      dist = min(dist, length(diff));
    }
  }

  out_FragColor = vec4(vec3(dist), 1.0);
}
