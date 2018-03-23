//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2014 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#include "PixelFormat.h"

#include "Exception.h"
#include "StringHelper.h"

#include <ostream>

using namespace std;

namespace lava {

ostream& operator <<(ostream& os, PixelFormat pf)
{
    os << getPixelFormatString(pf);
    return os;
}

string getPixelFormatString(PixelFormat pf)
{
    switch (pf) {
        case B5G6R5:
            return "B5G6R5";
        case B8G8R8:
            return "B8G8R8";
        case B8G8R8A8:
            return "B8G8R8A8";
        case B8G8R8X8:
            return "B8G8R8X8";
        case A8B8G8R8:
            return "A8B8G8R8";
        case X8B8G8R8:
            return "X8B8G8R8";
        case R5G6B5:
            return "R5G6B5";
        case R8G8B8:
            return "R8G8B8";
        case R8G8B8A8:
            return "R8G8B8A8";
        case R8G8B8X8:
            return "R8G8B8X8";
        case A8R8G8B8:
            return "A8R8G8B8";
        case X8R8G8B8:
            return "X8R8G8B8";
        case I8:
            return "I8";
        case I16:
            return "I16";
        case YCbCr422:
            return "YCbCr422";
        case YCbCr420p:
            return "YCbCr420p";
        case YCbCrJ420p:
            return "YCbCrJ420p";
        case YCbCrA420p:
            return "YCbCrA420p";
        case R32G32B32A32F:
            return "R32G32B32A32F";
        case I32F:
            return "I32F";
        case NO_PIXELFORMAT:
            return "NO_PIXELFORMAT";
        default:
            return "Unknown " + toString(int(pf));
    }
}

std::vector<std::string> getSupportedPixelFormats()
{
    std::vector<std::string> pixelFormatsVector;
    int itPixelFormat = 0;
    while((PixelFormat)itPixelFormat != NO_PIXELFORMAT){
        std::string format = getPixelFormatString((PixelFormat)itPixelFormat);
        pixelFormatsVector.push_back(format);
        itPixelFormat++;
    }
    return pixelFormatsVector;
}

bool pixelFormatIsColored(PixelFormat pf)
{
    return (pf != I8 && pf != I16 && pf != I32F);
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
        case R32G32B32A32F:
            return 16;
        case A8B8G8R8:
        case X8B8G8R8:
        case A8R8G8B8:
        case X8R8G8B8:
        case B8G8R8A8:
        case B8G8R8X8:
        case R8G8B8A8:
        case R8G8B8X8:
        case I32F:
            return 4;
        case R8G8B8:
        case B8G8R8:
            return 3;
        case B5G6R5:
        case R5G6B5:
        case I16:
            return 2;
        case I8:
            return 1;
        case YCbCr422:
            return 2;
        default:
            LAVA_ASSERT(false);
            return 0;
    }
}

}
