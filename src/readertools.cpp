#include "readertools.hpp"
#include <array>

#include "imgtools/filters.hpp"
#include "imgtools/imgtools.hpp"
#include "raylib.h"

const Range THETA_RANGE_H = {-20.0f, 20.0f, 1.0f};
const Range THETA_RANGE_V1 = {-90.0f, -70.0f, 1.0f};
const Range THETA_RANGE_V2 = {70.0f, 90.0f, 1.0f};
const float RHO_STEP = 1.0f;

const int BLOCK_WIDTH = 116;
const int BLOCK_HEIGHT = 116;
const int BLOCK_X1 = 0, BLOCK_X2 = 1148;
const int BLOCK_Y1 = 0, BLOCK_Y2 = 749;

const Rectangle BLOCKS[4] = {
    {BLOCK_X1, BLOCK_Y1, BLOCK_WIDTH, BLOCK_HEIGHT},
    {BLOCK_X2, BLOCK_Y1, BLOCK_WIDTH, BLOCK_HEIGHT},
    {BLOCK_X1, BLOCK_Y2, BLOCK_WIDTH, BLOCK_HEIGHT},
    {BLOCK_X2, BLOCK_Y2, BLOCK_WIDTH, BLOCK_HEIGHT},
};

std::array<Vector2, 4> GetCorners(const Image *image) {
    std::array<Vector2, 4> corners = {};

    int block_counter = 0;
    for (Rectangle block_rect : BLOCKS) {
        Image block_img = ImageCopy(*image);
        ImageCrop(&block_img, block_rect);

        ImageThreshold(&block_img, 90);
        ImageNormalizedGradient(&block_img);
        ImageThreshold(&block_img, 1);

        int diagonal = GetDiagonalLength(&block_img);
        PixelVector white_pixels = FilterImageThreshold(&block_img, 255);

        HoughParameterSpace pspace_h(&white_pixels, diagonal,
                                     THETA_RANGE_H, RHO_STEP, 0.5);
        HoughParameterSpace pspace_v1(&white_pixels, diagonal,
                                      THETA_RANGE_V1, RHO_STEP, 0.5);
        HoughParameterSpace pspace_v2(&white_pixels, diagonal,
                                      THETA_RANGE_V2, RHO_STEP, 0.5);
        
        Line max_h = *pspace_h.max;
        Line max_v1 = *pspace_v1.max;
        Line max_v2 = *pspace_v2.max;

        Line line1 = max_h;
        Line line2 = max_v1.count > max_v2.count ? max_v1 : max_v2;

        Vector2 intersection = IntersectionPoint(line1, line2);
        intersection.x += block_rect.x;
        intersection.y += block_rect.y;

        corners[block_counter] = intersection;
        
        block_counter++;
    }

    return corners;
}