#ifndef COLORMAP_H
#define COLORMAP_H

#include <stdint.h>

class ColorMap
{
public:
    static void jet(double *rgb, double x, double min, double max);
};

#endif // COLORMAP_H
