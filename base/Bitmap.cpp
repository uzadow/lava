#include "Bitmap.h"

#include "Exception.h"
#include "MathHelper.h"

#include <cstring>
#include <iostream>
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
    : Bitmap(vec2(size), pf)
{
}

Bitmap::Bitmap(ivec2 size, PixelFormat pf, uint8_t* pBits, int stride)
    : Bitmap(size, pf)
{
    initWithData(pBits, stride);
}

Bitmap::Bitmap(ivec2 size, PixelFormat pf, const vector<uint8_t*>& pPlanes,
        const std::vector<int>& strides)
    : Bitmap(size, pf)
{
    initWithData(pPlanes, strides);
}

Bitmap::Bitmap(const Bitmap& origBmp)
    : Bitmap(origBmp.getSize(), origBmp.getPixelFormat())
{
    initWithData(origBmp.m_pPlanes, origBmp.m_Strides);
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

int Bitmap::getLineLen(unsigned i) const
{
    return getPlaneSize(i).x*getBytesPerPixel();
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

BitmapPtr Bitmap::subtract(const Bitmap& otherBmp)
{
    LAVA_ASSERT(m_PF == otherBmp.m_PF);
    LAVA_ASSERT(m_Size == otherBmp.m_Size);

    BitmapPtr pResultBmp(new Bitmap(m_Size, m_PF));

    for (unsigned i=0; i<m_pPlanes.size(); ++i) {
        glm::vec2 planeSize = getPlaneSize(i);

        const uint8_t* pBits0 = m_pPlanes[i];
        int stride0 = m_Strides[i];

        const uint8_t* pBits1 = otherBmp.getPixels(i);
        int stride1 = otherBmp.getStride(i);

        uint8_t* pResultBits = pResultBmp->getPixels(i);
        int resultStride = pResultBmp->getStride(i);

        for (int y=0; y<planeSize.y; ++y) {
            const uint8_t * pSrcBits0 = pBits0;
            const uint8_t * pSrcBits1 = pBits1;
            uint8_t * pDestBits = pResultBits;

            for (int x=0; x<planeSize.x*getBytesPerPixel(); ++x) {
                *pDestBits = *pSrcBits0 - *pSrcBits1;
                pSrcBits0++;
                pSrcBits1++;
                pDestBits++;
            }
            pBits0 += stride0;
            pBits1 += stride1;
            pResultBits += resultStride;
        }
    }
    return pResultBmp;
}

float Bitmap::getAvg() const
{
    LAVA_ASSERT(m_PF != R5G6B5 && m_PF != B5G6R5);

    float sum = 0;
    int componentsPerPixel = 0;
    unsigned numComponents = 0;
    for (unsigned i=0; i<m_pPlanes.size(); ++i) {
        glm::vec2 planeSize = getPlaneSize(i);
        uint8_t * pSrc = m_pPlanes[i];

        for (int y=0; y<planeSize.y; ++y) {
            switch(m_PF) {
                case R8G8B8X8:
                case B8G8R8X8:
                    {
                        uint8_t * pSrcPixel = pSrc;
                        for (int x=0; x<planeSize.x; ++x) {
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
                        for (int x=0; x<planeSize.x; ++x) {
                            int alpha = pSrcPixel[3];
                            sum += ((pSrcPixel[0] + pSrcPixel[1] + pSrcPixel[2])*alpha)/255 + alpha;
                            pSrcPixel+=4;
                        }
                        componentsPerPixel = 4;
                    }
                    break;
                default:
                    {
                        uint8_t * pSrcComponent = pSrc;
                        for (int x=0; x<getLineLen(i); ++x) {
                            sum += *pSrcComponent;
                            pSrcComponent++;
                        }
                        componentsPerPixel = getBytesPerPixel();
                    }
            }
            pSrc += m_Strides[i];
        }
        numComponents += planeSize.x * planeSize.y * componentsPerPixel;
    }
    return sum/numComponents;
}

float Bitmap::getStdev() const
{
    float average = getAvg();
    float sum = 0;

    int componentsPerPixel = 0;
    unsigned numComponents = 0;
    for (unsigned i=0; i<m_pPlanes.size(); ++i) {
        uint8_t * pSrc = m_pPlanes[i];
        glm::vec2 planeSize = getPlaneSize(i);
        for (int y = 0; y < planeSize.y; ++y) {
            switch(m_PF) {
                case R8G8B8X8:
                case B8G8R8X8:
                    {
                        uint8_t * pSrcPixel = pSrc;
                        for (int x=0; x<planeSize.x; ++x) {
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
                        for (int x=0; x<planeSize.x; ++x) {
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
                default:
                    {
                        uint8_t * pSrcComponent = pSrc;
                        for (int x=0; x<getLineLen(i); ++x) {
                            sum += sqr(*pSrcComponent-average);
                            pSrcComponent++;
                        }
                        componentsPerPixel = getBytesPerPixel();
                    }
            }
            pSrc += m_Strides[i];
        }
        numComponents += planeSize.x * planeSize.y * componentsPerPixel;
    }
    sum /= numComponents;
    return ::sqrt(sum);
}

void Bitmap::dump(bool bDumpPixels) const
{
    cerr << "Bitmap: " << endl;
    cerr << "  m_Size: " << m_Size.x << "x" << m_Size.y << endl;
    cerr << "  m_PF: " << m_PF << endl;
    cerr << "  Pixel data: " << endl;
    for (unsigned p=0; p<m_pPlanes.size(); ++p) {
        cerr << "    m_pPlanes[" << p << "]:" << m_pPlanes[p] << endl;
        cerr << "      stride:" << m_Strides[p] << endl;


        ivec2 dumpSize;
        dumpSize = getPlaneSize(p);
        if (!bDumpPixels) {
            dumpSize = ivec2(glm::min(dumpSize[0],16), 1);
        }

        for (int y = 0; y < dumpSize.y; ++y) {
            uint8_t * pLine = m_pPlanes[p]+m_Strides[p]*y;
            cerr << "      ";
            for (int x = 0; x < dumpSize.x; ++x) {
                uint8_t * pPixel = pLine+getBytesPerPixel()*x;
                cerr << "[";
                for (int i = 0; i < getBytesPerPixel(); ++i) {
                    cerr << hex << setw(2) << (int)(pPixel[i]) << " ";
                }
                cerr << "]";
            }
            cerr << endl;
        }
    }
    cerr << dec;
}

int Bitmap::getPreferredStride(int width, PixelFormat pf)
{
    return (((width*lava::getBytesPerPixel(pf))-1)/4+1)*4;
}

void Bitmap::initWithData(const uint8_t* pBits, int stride)
{
    initPlaneWithData(0, pBits, stride);
}

void Bitmap::initWithData(const std::vector<uint8_t *>& pPlanes, const std::vector<int>& strides)
{
    for (unsigned i=0; i<m_pPlanes.size(); ++i) {
        initPlaneWithData(i, pPlanes[i], strides[i]);
    }
}

void Bitmap::initPlaneWithData(unsigned i, const uint8_t* pBits, int stride)
{
    if (m_Strides[i] == stride && stride == (m_Size.x*getBytesPerPixel())) {
        memcpy(m_pPlanes[i], pBits, stride*m_Size.y);
    } else {
        for (int y = 0; y < m_Size.y; ++y) {
            memcpy(m_pPlanes[i]+m_Strides[i]*y, pBits+stride*y, m_Strides[i]);
        }
    }
}


void Bitmap::allocBits()
{
    LAVA_ASSERT(m_pPlanes.empty());
    LAVA_ASSERT(m_Size.x > 0 && m_Size.y > 0);
    checkValidSize();
    if (pixelFormatIsPlanar(m_PF)) {
        LAVA_ASSERT(m_PF == YCbCr420p || m_PF == YCbCrJ420p || m_PF == YCbCrA420p);

        // Y Plane
        int stride = getPreferredStride(m_Size.x, m_PF);
        auto pBits = new uint8_t[size_t(stride)*m_Size.y];
        m_pPlanes.push_back(pBits);
        m_Strides.push_back(stride);

        // U, V Planes
        ivec2 uvSize = m_Size/2;
        stride = getPreferredStride(uvSize.x, m_PF);
        for (int i=0; i<2; ++i) {
            pBits = new uint8_t[size_t(stride)*uvSize.y];
            m_pPlanes.push_back(pBits);
            m_Strides.push_back(stride);
        }

        // A Plane
        if (m_PF == YCbCrA420p) {
            stride = getPreferredStride(m_Size.x, m_PF);
            pBits = new uint8_t[size_t(stride)*m_Size.y];
            m_pPlanes.push_back(pBits);
            m_Strides.push_back(stride);
        }
    } else {
        // Non-planar case
        int stride = getPreferredStride(m_Size.x, m_PF);
        auto pBits = new uint8_t[size_t(stride)*m_Size.y];
        m_pPlanes.push_back(pBits);
        m_Strides.push_back(stride);
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
