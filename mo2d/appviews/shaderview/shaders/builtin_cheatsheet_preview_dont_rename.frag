uniform vec2 u_resolution;


float none(float n) {
    return n;
}

void main() {
    vec2 st = (7.5 * (gl_FragCoord.xy / u_resolution)) - (7.5 / 2.0);
    float x = st.x;
    float y = fn(x);
    vec3 col = vec3(0.2); // default dark backdrop

    // grid line if st near-integral
    vec2 f = abs(fract(st));
    if (((f.x <= 0.01) || (f.x >= 0.09)) && ((f.y <= 0.01) || (f.y >= 0.09))) { col = vec3(0.1); }
    // backdrop: lighter if st -1..1
    if ((st.x >= -1.0) && (st.x <= 1.0) && (st.y >= -1.0) && (st.y <= 1.0)) { col = vec3(0.4); }
    // plot
    if ((st.y >= (y - 0.044)) && (st.y <= (y + 0.044))) { col = vec3(0.8, 0.6, 0.2); }

    gl_FragColor = vec4(col, 1.0);
}
