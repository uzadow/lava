#pragma once

#include <string>
#include <vector>

namespace lava {

// TODO: Scoped enum

typedef enum {
    B5G6R5, B8G8R8, B8G8R8A8, B8G8R8X8, A8B8G8R8, X8B8G8R8,
    R5G6B5, R8G8B8, R8G8B8A8, R8G8B8X8, A8R8G8B8, X8R8G8B8,
    I8, I16,
    YCbCr422,  
    YCbCr420p, 
    YCbCrJ420p,
    YCbCrA420p,
    R32G32B32A32F, // 32bit per channel float rgba
    I32F,
    NO_PIXELFORMAT
} PixelFormat;

std::ostream& operator <<(std::ostream& os, PixelFormat pf);

std::string getPixelFormatString(PixelFormat pf);
std::vector<std::string> getSupportedPixelFormats();
bool pixelFormatIsColored(PixelFormat pf);
bool pixelFormatHasAlpha(PixelFormat pf);
bool pixelFormatIsPlanar(PixelFormat pf);
bool pixelFormatIsBlueFirst(PixelFormat pf);
unsigned getNumPixelFormatPlanes(PixelFormat pf);
unsigned getBytesPerPixel(PixelFormat pf);

}