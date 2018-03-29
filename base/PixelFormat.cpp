#include "PixelFormat.h"

#include "Exception.h"
#include "StringHelper.h"

#include <ostream>

using namespace std;

namespace lava {

ostream& operator <<(ostream& os, PixelFormat pf)
{
    switch (pf) {
        case B5G6R5:
            os << "B5G6R5";
            break;
        case B8G8R8:
            os << "B8G8R8";
            break;
        case B8G8R8A8:
            os << "B8G8R8A8";
            break;
        case B8G8R8X8:
            os << "B8G8R8X8";
            break;
        case A8B8G8R8:
            os << "A8B8G8R8";
            break;
        case X8B8G8R8:
            os << "X8B8G8R8";
            break;
        case R5G6B5:
            os << "R5G6B5";
            break;
        case R8G8B8:
            os << "R8G8B8";
            break;
        case R8G8B8A8:
            os << "R8G8B8A8";
            break;
        case R8G8B8X8:
            os << "R8G8B8X8";
            break;
        case A8R8G8B8:
            os << "A8R8G8B8";
            break;
        case X8R8G8B8:
            os << "X8R8G8B8";
            break;
        case I8:
            os << "I8";
            break;
        case YCbCr422:
            os << "YCbCr422";
            break;
        case YCbCr420p:
            os << "YCbCr420p";
            break;
        case YCbCrJ420p:
            os << "YCbCrJ420p";
            break;
        case YCbCrA420p:
            os << "YCbCrA420p";
            break;
        case NO_PIXELFORMAT:
            os << "NO_PIXELFORMAT";
            break;
        default:
            os << "Unknown " << int(pf);
            break;
    }
    return os;
}

bool pixelFormatIsColored(PixelFormat pf)
{
    return (pf != I8);
}

bool pixelFormatHasAlpha(PixelFormat pf)
{
    return pf == B8G8R8A8 || pf == A8B8G8R8 || pf == R8G8B8A8 || pf == A8R8G8B8 ||
            pf == YCbCrA420p;
}

bool pixelFormatIsPlanar(PixelFormat pf)
{
    return pf == YCbCr420p || pf == YCbCrJ420p || pf == YCbCrA420p;
}

bool pixelFormatIsBlueFirst(PixelFormat pf)
{
    return pf == B5G6R5 || pf == B8G8R8 || pf == B8G8R8X8 || pf == B8G8R8A8;
}

unsigned getNumPixelFormatPlanes(PixelFormat pf)
{
    switch (pf) {
        case YCbCr420p:
        case YCbCrJ420p:
            return 3;
        case YCbCrA420p:
            return 4;
        default:
            return 1;
    }
}

unsigned getBytesPerPixel(PixelFormat pf)
{
    switch (pf) {
        case A8B8G8R8:
        case X8B8G8R8:
        case A8R8G8B8:
        case X8R8G8B8:
        case B8G8R8A8:
        case B8G8R8X8:
        case R8G8B8A8:
        case R8G8B8X8:
            return 4;
        case R8G8B8:
        case B8G8R8:
            return 3;
        case B5G6R5:
        case R5G6B5:
            return 2;
        case I8:
        case YCbCr420p:
        case YCbCrJ420p:
        case YCbCrA420p:
            return 1;
        case YCbCr422:
            return 2;
        default:
            LAVA_ASSERT(false);
            return 0;
    }
}

}
