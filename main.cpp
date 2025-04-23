#include <cmath>
#include <cstddef>
#include <cstdio>

#include "raylib.h"
#include "sane/sane.h"
#include "scanner.hpp"

int main() {
    printf("initializing sane...\n");
    SANE_Int sane_version;
    sane_init(&sane_version, NULL);

    Image img = ImageFromScanner("000000000YP76T4DPR1a");
    ExportImage(img, "scan.png");

    printf("exiting sane...\n");
    sane_exit();
}