uniform vec2 u_resolution;


float none(float n) {
    return n;
}

void main() {
    vec2 st = (10.0 * (gl_FragCoord.xy / u_resolution)) - 5.0;
    float x = st.x;
    float y = fn(x);
    vec3 col = vec3(0.2); // default dark backdrop

    // backdrop: lighter if st -1..1
    if ((st.x > -1.0) && (st.x < 1.0) && (st.y > -1.0) && (st.y < 1.0)) { col = vec3(0.4); }
    // black cross if st.x or st.y 0
    if (((st.x > -0.01) && (st.x < 0.01)) || ((st.y > -0.01) && (st.y < 0.01))) { col = vec3(0.1); }
    // plot
    if ((st.y >= (y - 0.044)) && (st.y <= (y + 0.044))) { col = vec3(0.8); }

    gl_FragColor = vec4(col, 1.0);
}
