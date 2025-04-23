#include "scanner.hpp"

#include <sane/sane.h>

#include <cmath>
#include <cstdint>

const float SCANNER_HEIGHT = 24969216.0f;
const float SCANNER_WIDTH = 14149222.0f;
const float SCANNER_RATIO = SCANNER_HEIGHT / SCANNER_WIDTH;
const float A4_RATIO = std::sqrt(2.0f);
const char *VALUE_TYPES_STR[6]{"SANE_TYPE_BOOL",   "SANE_TYPE_INT",    "SANE_TYPE_FIXED",
                               "SANE_TYPE_STRING", "SANE_TYPE_BUTTON", "SANE_TYPE_GROUP"};

void PrintDeviceOpts(SANE_Handle handle) {
    int num_opts;
    sane_control_option(handle, 0, SANE_ACTION_GET_VALUE, &num_opts, NULL);
    printf("Found %d options:\n", num_opts);


    for (int opt = 1; opt < num_opts; opt++) {
        const SANE_Option_Descriptor *opt_descriptor = sane_get_option_descriptor(handle, opt);
        if (opt_descriptor->type == 5) {
            printf("Group: %s\n", opt_descriptor->title);
            continue;
        }
        printf("\t%02d - %s\n", opt, opt_descriptor->title);
        printf("\t     name: %s\n", opt_descriptor->name);
        printf("\t     desc: %s\n", opt_descriptor->desc);
        printf("\t     value_type: %s\n", VALUE_TYPES_STR[opt_descriptor->type]);

        switch (opt_descriptor->type) {
            case SANE_TYPE_BOOL: {
                bool v;
                sane_control_option(handle, opt, SANE_ACTION_GET_VALUE, (void *)&v, NULL);
                printf("\t     value: %s", v ? "true" : "false\n");
                break;
            }
            case SANE_TYPE_FIXED:  // same as int
            case SANE_TYPE_INT: {
                int v;
                sane_control_option(handle, opt, SANE_ACTION_GET_VALUE, (void *)&v, NULL);
                printf("\t     value: %d\n", v);
                break;
            }
            case SANE_TYPE_STRING: {
                char *v = (char *)malloc(1);  // SILENCE THE WARNINGS AAAA
                sane_control_option(handle, opt, SANE_ACTION_GET_VALUE, (void *)v, NULL);
                printf("\t     value: %s\n", v);
                break;
            }
            default: {
            }
        }
    }
}

// Returns list of scanner names
std::vector<std::string> GetScannerNames() {
    std::vector<std::string> scanners{};

    SANE_Device **devices;
    sane_get_devices((const SANE_Device ***)&devices, true);

    while ((*devices) != NULL) {
        scanners.push_back((*devices)->name);
        devices++;
    }

    return scanners;
}

// Returns a handle to the scanner. NULL if no scanner was found.
SANE_Handle GetScanner(std::string name_pattern) {
    std::vector<std::string> scanners = GetScannerNames();

    // Tries to find the device
    for (std::string scanner_name : scanners) {
        if (scanner_name.find(name_pattern) != (size_t)-1) {
            SANE_Handle device_handle;
            SANE_Status status = sane_open(scanner_name.c_str(), &device_handle);
            if (status != SANE_STATUS_GOOD) { return nullptr; }
            return device_handle;
        }
    }

    // Couldn't find the device
    return nullptr;
}

// Returns a pointer to data read from a scanner
unsigned char *ReadScannerData(SANE_Handle handle) {
    SANE_Parameters p;
    sane_get_parameters(handle, &p);

    int bytes = p.lines * p.bytes_per_line;
    uint8_t *pixels = (uint8_t *)malloc(bytes);
    uint8_t *img_start = pixels;
    for (int p = 0; p < bytes; p++) { pixels[p] = 255; }

    int read_length;
    while (true) {
        SANE_Status status = sane_read(handle, pixels, bytes, &read_length);
        if (status == SANE_STATUS_EOF) {
            break;
        } else if (status != SANE_STATUS_GOOD) {
            sane_close(handle);
            return nullptr;
        }
        pixels += read_length;
    }

    return img_start;
}

// Reads data from a scanner and converts it into a raylib image
Image ImageFromScanner(SANE_Handle device_handle) {
    // sets SCAN MODE option to monochrome
    sane_control_option(device_handle, 2, SANE_ACTION_SET_VALUE, (void *)"Gray", NULL);
    // sets SCAN RESOLUTION option to 150dpi
    const int SCAN_DPI = 150;
    sane_control_option(device_handle, 3, SANE_ACTION_SET_VALUE, (void *)&SCAN_DPI, NULL);

    // Starts scanning
    SANE_Status status = sane_start(device_handle);
    if (status != SANE_STATUS_GOOD) {
        sane_close(device_handle);
        return Image{};
    }

    // Trims the scan to fit an A4-sized image (doing it after the read
    // gives a different result for some reason)
    SANE_Parameters p;
    sane_get_parameters(device_handle, &p);
    int height = (float)p.lines / (SCANNER_RATIO / A4_RATIO);

    // Reads data from the scanner
    uint8_t *img_start = ReadScannerData(device_handle);
    if (img_start == nullptr) {
        sane_close(device_handle);
        return Image{};
    }

    // Converts the data to a raylib image
    Image img{img_start, p.bytes_per_line, height, 1, PIXELFORMAT_UNCOMPRESSED_GRAYSCALE};
    sane_close(device_handle);

    return img;
}