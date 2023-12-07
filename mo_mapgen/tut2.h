#include "perlin.h"
#include <cstdio>


const auto dirPathTut1 = "/home/_/tmp/tests/tut2";


void tut2() {
    noise::module::Perlin module_perlin;
    double value = module_perlin.GetValue(1.25, 0.75, 0.5);
    printf("%f\n", value);
}
