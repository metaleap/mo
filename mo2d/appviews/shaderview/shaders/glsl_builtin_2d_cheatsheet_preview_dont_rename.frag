uniform vec2 u_resolution;

out vec4 out_FragColor;


void main() {
    vec2 st = (7.5 * (gl_FragCoord.xy / u_resolution)) - (7.5 / 2.0);
    vec3 col = vec3(fn(st.x, st.y));

    // grid line if st nearly integer
    vec2 f = abs(fract(st));
    if (((f.x <= 0.02) || (f.x >= 0.98)) || ((f.y <= 0.02) || (f.y >= 0.98))) { col = vec3(0.8, 0.6, 0.2); }

    out_FragColor = vec4(col, 1.0);
}
