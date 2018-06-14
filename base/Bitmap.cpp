#include "Bitmap.h"

#include "Exception.h"
#include "MathHelper.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace glm;

namespace lava {

float Bitmap::s_MaxAvgDiff = 0.5;
float Bitmap::s_MaxStdevDiff = 2;
tjhandle Bitmap::s_JPEGDecompressor = 0;

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

Bitmap Bitmap::load(const string& sFilename)
{
    int w, h;

    if (!s_JPEGDecompressor) {
        s_JPEGDecompressor = tjInitDecompress();
    }

    string sCompressedImg;
    {
        ifstream is(sFilename.c_str());
        if (!is.good()) {
            throw Exception("Error loading '" + sFilename + "': Could not open file.");
        }
        stringstream sstr;
        sstr << is.rdbuf();
        sCompressedImg = sstr.str();
    }

    int subsamp;
    int colorspace;
    uint8_t* pCompressedImg = (uint8_t*)sCompressedImg.c_str();
    int len = sCompressedImg.size();

    int err = tjDecompressHeader3(s_JPEGDecompressor, pCompressedImg, len, &w, &h, &subsamp, &colorspace);
    if (err == 0 && colorspace == TJCS_YCbCr && subsamp == TJSAMP_420) {
        // This is a YUV 4:2:0 jpeg. Load without conversion to RGB for speed using TurboJPEG.
        Bitmap bmp(ivec2(w, h), YCbCr420p);
        uint8_t** pPlanes = bmp.m_pPlanes.data();
        int* pStrides = bmp.m_Strides.data();
        err = tjDecompressToYUVPlanes(s_JPEGDecompressor, pCompressedImg, len, pPlanes, w, pStrides, h, 0);
        if (err == -1) {
            throw Exception("Error loading '" + sFilename + "': " + tjGetErrorStr());
        }
        return bmp;
    } else {
        // Non-optimized decode for all other image formats.

        int numChannels;
        PixelFormat pf;
        uint8_t* pData = stbi_load(sFilename.c_str(), &w, &h, &numChannels, 0);
        if (!pData) {
            throw Exception("Error loading '" + sFilename + "': " + stbi_failure_reason());
        }
        switch (numChannels) {
            case 1:
                pf = I8;
                break;
            case 3:
                pf = R8G8B8;
                break;
            case 4:
                pf = R8G8B8A8;
                break;
            default:
                pf = NO_PIXELFORMAT;
                LAVA_ASSERT(false);
        }
        return Bitmap(ivec2(w, h), pf, pData, w*numChannels);
    }
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
                    if (memcmp(pDest, pSrc, size_t(lineLen)) != 0) {
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

    int sum = 0;
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
    return float(sum)/numComponents;
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
    return ::sqrt(double(sum/numComponents));
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
    if (pixelFormatIsPlanar(m_PF)) {
        LAVA_ASSERT(m_PF == YCbCr420p || m_PF == YCbCrJ420p || m_PF == YCbCrA420p);

        allocPlane(m_Size); // Y Plane

        ivec2 uvSize = (m_Size+ivec2(1,1))/2;  //Half size, round up.
        allocPlane(uvSize); // U,V Planes
        allocPlane(uvSize);

        if (m_PF == YCbCrA420p) {
            allocPlane(m_Size); // A Plane
        }
    } else {
        // Non-planar case
        allocPlane(m_Size);
    }
}

void Bitmap::allocPlane(const ivec2& size)
{
    int stride = getPreferredStride(size.x, m_PF);
    auto pBits = new uint8_t[size_t(stride)*size.y];
    m_pPlanes.push_back(pBits);
    m_Strides.push_back(stride);
}

void Bitmap::setEpsilon(float maxAvgDiff, float maxStdevDiff)
{
    s_MaxAvgDiff = maxAvgDiff;
    s_MaxStdevDiff = maxStdevDiff;
}

bool Bitmap::almostEqual(const Bitmap& bmp1, const Bitmap& bmp2)
{
    if (bmp1.getSize() != bmp2.getSize()) {
        return false;
    }
    if (bmp1.getPixelFormat() != bmp2.getPixelFormat()) {
        return false;
    }
    if (fabs(bmp1.getAvg() - bmp2.getAvg()) > s_MaxAvgDiff) {
        return false;
    }
    if (fabs(bmp1.getStdev() - bmp2.getStdev()) > s_MaxStdevDiff) {
        return false;
    }
    return true;
}

};
