#include <string>
#include <vector>

#include "raylib.h"
#include "sane/sane.h"

// DON'T USE! This makes SANE crash when closing the device.
void PrintDeviceOpts(SANE_Handle handle);

// Returns list of scanner names
std::vector<std::string> GetScanners();

// Returns a handle to the scanner. NULL if no scanner was found.
SANE_Handle FindScanner(std::string name_pattern);

// Returns a pointer to data read from a scanner
unsigned char *ReadScannerData(SANE_Handle handle);

// Reads data from a scanner and converts it into a raylib image
Image ImageFromScanner(std::string name_pattern);