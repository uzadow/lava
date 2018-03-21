#include "Bitmap.h"

#include "Exception.h"
#include "MathHelper.h"

#include <cstring>
#include <iostream>
//#include <iomanip>
#include <stdlib.h>

using namespace std;
using namespace glm;

namespace lava {

Bitmap::Bitmap(vec2 size, PixelFormat pf)
    : m_Size(size),
      m_PF(pf)
{
    allocBits();
}

Bitmap::Bitmap(ivec2 size, PixelFormat pf)
    : m_Size(size),
      m_PF(pf)
{
    allocBits();
}

Bitmap::Bitmap(ivec2 size, PixelFormat pf, uint8_t* pBits, int stride)
    : m_Size(size),
      m_PF(pf)
{
    initWithData(pBits, stride);
}

Bitmap::Bitmap(ivec2 size, PixelFormat pf, const vector<uint8_t*>& pPlanes,
        const std::vector<int>& strides)
    : m_Size(size),
      m_PF(pf)
{
    initWithData(pPlanes, strides);
}

Bitmap::Bitmap(const Bitmap& origBmp)
    : m_Size(origBmp.getSize()),
      m_PF(origBmp.getPixelFormat())
{
    initWithData(origBmp.getPlanes(), origBmp.getStrides());
}

Bitmap::~Bitmap()
{
    for (auto pBits: m_pPlanes) {
        delete[] pBits;
    }
    m_pPlanes.clear();
}

void Bitmap::load(const string& sFilename)
{
}

void Bitmap::save(const string& sFilename) const
{

}

ivec2 Bitmap::getSize() const
{
    return m_Size;
}
    
ivec2 Bitmap::getPlaneSize(unsigned i) const
{
    LAVA_ASSERT(i < m_pPlanes.size());
    // We're assuming all planar bitmaps are YCbCr420 or YCbCrA420 
    if (i == 0 || i == 3) {
        return m_Size;
    } else {
        return m_Size/2;
    }

}

int Bitmap::getStride(unsigned i) const
{
    LAVA_ASSERT(i < m_Strides.size());
    return m_Strides[i];
}

PixelFormat Bitmap::getPixelFormat() const
{
    return m_PF;
}

uint8_t* Bitmap::getPixels(unsigned i)
{
    LAVA_ASSERT(i < m_Strides.size());
    return m_pPlanes[i];
}

const uint8_t* Bitmap::getPixels(unsigned i) const
{
    LAVA_ASSERT(i < m_Strides.size());
    return m_pPlanes[i];
}

int Bitmap::getBytesPerPixel() const
{
    return lava::getBytesPerPixel(m_PF);
}

int Bitmap::getLineLen() const
{
    return m_Size.x*getBytesPerPixel();
}

int Bitmap::getMemNeeded() const
{
    LAVA_ASSERT(!pixelFormatIsPlanar(m_PF)); // TODO
    return m_Strides[0]*m_Size.y;
}

bool Bitmap::operator ==(const Bitmap& otherBmp)
{
    // We allow Stride to be different here, since we're looking for equal value only.
    if (m_Size != otherBmp.m_Size || m_PF != otherBmp.m_PF) {
        return false;
    }

    for (unsigned i=0; i<m_pPlanes.size(); ++i) {
        ivec2 size = getPlaneSize(i);

        const uint8_t * pSrc = otherBmp.getPixels(i);
        uint8_t * pDest = m_pPlanes[i];
        int lineLen = size.x*getBytesPerPixel();
        for (int y = 0; y < size.y; ++y) {
            switch(m_PF) {
                case R8G8B8X8:
                case B8G8R8X8:
                    for (int x = 0; x < size.x; ++x) {
                        const uint8_t * pSrcPixel = pSrc+x*getBytesPerPixel();
                        uint8_t * pDestPixel = pDest+x*getBytesPerPixel();
                        if (*pDestPixel != *pSrcPixel && 
                                *(pDestPixel+1) != *(pSrcPixel+1) &&
                                *(pDestPixel+2) != *(pSrcPixel+2) ) 
                        {
                            return false;
                        }
                    }
                    break;
                default:
                    if (memcmp(pDest, pSrc, lineLen) != 0) {
                        return false;
                    }
            }
            pDest += m_Strides[i];
            pSrc += otherBmp.getStride(i);
        }
    }
    return true;
}


float Bitmap::getAvg() const
{
    LAVA_ASSERT(!pixelFormatIsPlanar(m_PF));

    float sum = 0;
    uint8_t * pSrc = m_pPlanes[0];
    int componentsPerPixel;
    for (int y = 0; y < getSize().y; ++y) {
        switch(m_PF) {
            case R8G8B8X8:
            case B8G8R8X8:
                {
                    uint8_t * pSrcPixel = pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        sum += pSrcPixel[0] + pSrcPixel[1] + pSrcPixel[2];
                        pSrcPixel+=4;
                    }
                    componentsPerPixel = 3;
                }
                break;
            case R8G8B8A8:
            case B8G8R8A8:
                {
                    uint8_t * pSrcPixel = pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        int alpha = pSrcPixel[3];
                        sum += ((pSrcPixel[0] + pSrcPixel[1] + pSrcPixel[2])*alpha)/255 + alpha;
                        pSrcPixel+=4;
                    }
                    componentsPerPixel = 4;
                }
                break;
            case I16:
            case R32G32B32A32F:
            case I32F:
                LAVA_ASSERT(false);
                break;
            default:
                {
                    uint8_t * pSrcComponent = pSrc;
                    for (int x = 0; x < getLineLen(); ++x) {
                        sum += *pSrcComponent;
                        pSrcComponent++;
                    }
                    componentsPerPixel = getBytesPerPixel();
                }
        }
        pSrc += m_Strides[0];
    }
    sum /= componentsPerPixel;
    return sum/(m_Size.x*m_Size.y);
}

float Bitmap::getStdDev() const
{
    LAVA_ASSERT(!pixelFormatIsPlanar(m_PF));

    float average = getAvg();
    float sum = 0;

    uint8_t * pSrc = m_pPlanes[0];
    int componentsPerPixel;
    for (int y = 0; y < getSize().y; ++y) {
        switch(m_PF) {
            case R8G8B8X8:
            case B8G8R8X8:
                {
                    uint8_t * pSrcPixel = pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        sum += sqr(pSrcPixel[0]-average);
                        sum += sqr(pSrcPixel[1]-average);
                        sum += sqr(pSrcPixel[2]-average);
                        pSrcPixel += 4;
                    }
                    componentsPerPixel = 3;
                }
                break;
            case R8G8B8A8:
            case B8G8R8A8:
                {
                    uint8_t * pSrcPixel = pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        int alpha = pSrcPixel[3];
                        if (alpha > 0) {
                            sum += sqr(pSrcPixel[0]-average);
                            sum += sqr(pSrcPixel[1]-average);
                            sum += sqr(pSrcPixel[2]-average);
                            sum += sqr(pSrcPixel[3]-average);
                        }
                        pSrcPixel += 4;
                    }
                    componentsPerPixel = 4;
                }
                break;
            case I16:
            case R32G32B32A32F:
            case I32F:
                LAVA_ASSERT(false);
                break;
            default:
                {
                    uint8_t * pSrcComponent = pSrc;
                    for (int x = 0; x < getLineLen(); ++x) {
                        sum += sqr(*pSrcComponent-average);
                        pSrcComponent++;
                    }
                    componentsPerPixel = getBytesPerPixel();
                }
        }
        pSrc += m_Strides[0];
    }
    sum /= componentsPerPixel;
    sum /= m_Size.x*m_Size.y;
    return ::sqrt(sum);
}

void Bitmap::dump(bool bDumpPixels) const
{
    cerr << "Bitmap: " << endl;
    cerr << "  m_Size: " << m_Size.x << "x" << m_Size.y << endl;
    cerr << "  m_PF: " << getPixelFormatString(m_PF) << endl;
    cerr << "  m_Strides: " << m_Strides << endl;
    cerr << "  m_pPlanes: " << m_pPlanes << endl;
    cerr << "  Pixel data: " << endl;
    for (unsigned p=0; p<m_pPlanes.size(); ++p) {
        cerr << "    Plane " << p << endl;

        ivec2 max;
        if (bDumpPixels) {
            max = getPlaneSize(p);
        } else {
            max = ivec2(16,1);
        }

        for (int y = 0; y < max.y; ++y) {
            uint8_t * pLine = m_pPlanes[p]+m_Strides[p]*y;
            cerr << "      ";
            for (int x = 0; x < max.x; ++x) {
                if (m_PF == R32G32B32A32F) {
                    float * pPixel = (float*)(pLine+getBytesPerPixel()*x);
                    cerr << "[";
                    for (int i = 0; i < 4; ++i) {
                        cerr << setw(4) << setprecision(2) << pPixel[i] << " ";
                    }
                    cerr << "]";
                } else {
                    uint8_t * pPixel = pLine+getBytesPerPixel()*x;
                    cerr << "[";
                    for (int i = 0; i < getBytesPerPixel(); ++i) {
                        cerr << hex << setw(2) << (int)(pPixel[i]) << " ";
                    }
                    cerr << "]";
                }
            }
            cerr << endl;
        }
    cerr << dec;
}

int Bitmap::getPreferredStride(int width, PixelFormat pf)
{
    return (((width*avg::getBytesPerPixel(pf))-1)/4+1)*4;
}

void Bitmap::initWithData(const uint8_t* pBits, int stride)
{
//    cerr << "Bitmap::initWithData()" << endl;
    if (m_PF == YCbCr422) {
        if (m_Size.x%2 == 1) {
            AVG_LOG_WARNING("Odd size for YCbCr bitmap.");
            m_Size.x++;
        }
        if (m_Size.y%2 == 1) {
            AVG_LOG_WARNING("Odd size for YCbCr bitmap.");
            m_Size.y++;
        }
        if (m_Size.x%2 == 1 || m_Size.y%2 == 1) {
            AVG_LOG_ERROR("Odd size for YCbCr bitmap.");
        }
    }
    if (bCopyBits) {
        allocBits();
        if (m_Stride == stride && stride == (m_Size.x*getBytesPerPixel())) {
            memcpy(m_pBits, pBits, stride*m_Size.y);
        } else {
            for (int y = 0; y < m_Size.y; ++y) {
                memcpy(m_pBits+m_Stride*y, pBits+stride*y, m_Stride);
            }
        }
        m_bOwnsBits = true;
    } else {
        m_pBits = pBits;
        m_Stride = stride;
        m_bOwnsBits = false;
    }
}

void Bitmap::allocBits()
{
    LAVA_ASSERT(m_pPlanes.empty());
    LAVA_ASSERT(m_Size.x > 0 && m_Size.y > 0);
    checkValidSize();
    m_Stride = getPreferredStride(m_Size.x, m_PF);
    if (pixelFormatIsPlanar(m_PF)) {
        LAVA_ASSERT(m_PF == YCbCr420p || m_PF == YCbCrJ420p || m_PF == YCbCrA420p);

        // Y Plane
        pBits = new uint8_t[size_t(m_Stride)*m_Size.y];
        m_pPlanes.push_back(pBits);

        // U, V Planes
        ivec2 uvSize = m_Size/2;
        for (i=0; i<2; ++i) {
            pBits = new uint8_t[size_t(m_Stride)*uvSize.y];
            m_pPlanes.push_back(pBits);
        }

        // A Plane
        if (m_PF == YCbCrA420p) {
            pBits = new uint8_t[size_t(m_Stride)*m_Size.y];
            m_pPlanes.push_back(pBits);

        }
    } else {
        // Non-planar case
        pBits = new uint8_t[size_t(m_Stride)*m_Size.y];
        m_pPlanes.push_back(pBits);
    }
}

void Bitmap::checkValidSize() const
{
    switch (m_PF) {
        case YCbCr422:
            LAVA_ASSERT_MSG(m_Size.x%2 == 0, "Odd width for YCbCr422 bitmap.");
            break;
        case YCbCr420p:
        case YCbCrJ420p:
        case YCbCrA420p:
            LAVA_ASSERT_MSG((m_Size.x%2 == 0 && m_Size.y%2 == 0), "Odd size for YCbCr420 bitmap.");
            break;
        default:
            break;
    }
}

};
