#include "Bitmap.h"

#include "Exception.h"
//#include "FileHelper.h"

//#include <gdk-pixbuf/gdk-pixbuf.h>

#include <cstring>
#include <iostream>
#include <iomanip>
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
/*
    Bitmap* pTempBmp;
    switch (m_PF) {
        case B8G8R8X8:
            pTempBmp = new Bitmap(m_Size, R8G8B8);
            for (size_t y = 0; y < size_t(m_Size.y); y++) {
                uint8_t * pSrcLine = m_pBits + y*m_Stride;
                uint8_t * pDestLine = pTempBmp->getPixels() + 
                        y*pTempBmp->getStride();
                for (size_t x = 0; x < size_t(m_Size.x); x++) { 
                    pDestLine[x*3] = pSrcLine[x*4 + 2];
                    pDestLine[x*3 + 1] = pSrcLine[x*4 + 1];
                    pDestLine[x*3 + 2] = pSrcLine[x*4];
                }
            }
            break;
        case B8G8R8A8:
            pTempBmp = new Bitmap(m_Size, R8G8B8A8);
            for (size_t y = 0; y < size_t(m_Size.y); y++) {
                uint8_t * pSrcLine = m_pBits+y * m_Stride;
                uint8_t * pDestLine = pTempBmp->getPixels() + 
                        y*pTempBmp->getStride();
                for (size_t x = 0; x < size_t(m_Size.x); x++) { 
                    pDestLine[x*4] = pSrcLine[x*4 + 2];
                    pDestLine[x*4 + 1] = pSrcLine[x*4 + 1];
                    pDestLine[x*4 + 2] = pSrcLine[x*4];
                    pDestLine[x*4 + 3] = pSrcLine[x*4+3];
                }
            }
            break;
        case B8G8R8:
            pTempBmp = new Bitmap(m_Size, R8G8B8);
            for (size_t y = 0; y < size_t(m_Size.y); y++) {
                uint8_t * pSrcLine = m_pBits+y * m_Stride;
                uint8_t * pDestLine = pTempBmp->getPixels() + 
                        y*pTempBmp->getStride();
                for (size_t x = 0; x < size_t(m_Size.x); x++) { 
                    pDestLine[x*3] = pSrcLine[x*3 + 2];
                    pDestLine[x*3 + 1] = pSrcLine[x*3 + 1];
                    pDestLine[x*3 + 2] = pSrcLine[x*3];
                }
            }
            break;
        default:
            if (hasAlpha()) {
                pTempBmp = new Bitmap(m_Size, R8G8B8A8);
            } else {
                pTempBmp = new Bitmap(m_Size, R8G8B8);
            }
            pTempBmp->copyPixels(*this);
    }
    GdkPixbuf* pPixBuf = gdk_pixbuf_new_from_data(pTempBmp->getPixels(), 
            GDK_COLORSPACE_RGB, pTempBmp->hasAlpha(), 8, m_Size.x, m_Size.y, 
            pTempBmp->getStride(), 0, 0);

    string sExt = getExtension(sFilename);
    if (sExt == "jpg") {
        sExt = "jpeg";
    }

    GError* pError = 0;
    gboolean bOk = gdk_pixbuf_save(pPixBuf, sFilename.c_str(), sExt.c_str(), &pError, 
            NULL);
    g_object_unref(pPixBuf);
    if (!bOk) {
        string sErr = pError->message;
        g_error_free(pError);
        throw Exception(AVG_ERR_FILEIO, sErr);
    }

    delete pTempBmp;
    */
}

glm::ivec2 Bitmap::getSize() const
{
    return m_Size;
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
    LAVA_ASSERT(!pixelFormatIsPlanar(m_PF)); // TODO

    // We allow Stride to be different here, since we're looking for equal value only.
    if (m_Size != otherBmp.m_Size || m_PF != otherBmp.m_PF) {
        return false;
    }
    
    const uint8_t * pSrc = otherBmp.getPixels();
    uint8_t * pDest = m_pBits;
    int lineLen = getLineLen();
    for (int y = 0; y < getSize().y; ++y) {
        switch(m_PF) {
            case R8G8B8X8:
            case B8G8R8X8:
                for (int x = 0; x < getSize().x; ++x) {
                    const uint8_t * pSrcPixel = pSrc+x*getBytesPerPixel();
                    uint8_t * pDestPixel = pDest+x*getBytesPerPixel();
                    if (*((Pixel24*)(pDestPixel)) != *((Pixel24*)(pSrcPixel))) {
                        return false;
                    }
                }
                break;
            default:
                if (memcmp(pDest, pSrc, lineLen) != 0) {
                    return false;
                }
        }
        pDest += m_Stride;
        pSrc += otherBmp.getStride();
    }
    return true;
}


float Bitmap::getAvg() const
{
    float sum = 0;
    uint8_t * pSrc = m_pBits;
    int componentsPerPixel = getBytesPerPixel();
    for (int y = 0; y < getSize().y; ++y) {
        switch(m_PF) {
            case R8G8B8X8:
            case B8G8R8X8:
                {
                    Pixel32 * pSrcPixel = (Pixel32 *)pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        sum += pSrcPixel->getR()+pSrcPixel->getG()+pSrcPixel->getB();
                        pSrcPixel++;
                    }
                    componentsPerPixel = 3;
                }
                break;
            case I16:
                {
                    componentsPerPixel = 1;
                    unsigned short * pSrcPixel = (unsigned short *)pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        sum += *pSrcPixel;
                        pSrcPixel++;
                    }
                }
                break;
            case R8G8B8A8:
            case B8G8R8A8:
                {
                    Pixel32 * pSrcPixel = (Pixel32 *)pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        int a = pSrcPixel->getA();
                        if (a > 0) {
                            sum += ((pSrcPixel->getR()+pSrcPixel->getG()+
                                    pSrcPixel->getB())*a)/255+pSrcPixel->getA();
                        }
                        pSrcPixel++;
                    }
                    componentsPerPixel = 4;
                }
                break;
            default:
                {
                    uint8_t * pSrcComponent = pSrc;
                    for (int x = 0; x < getLineLen(); ++x) {
                        sum += *pSrcComponent;
                        pSrcComponent++;
                    }
                }
        }
        pSrc += m_Stride;
    }
    sum /= componentsPerPixel;
    return sum/(getSize().x*getSize().y);
}

float Bitmap::getStdDev() const
{
    float average = getAvg();
    float sum = 0;

    uint8_t * pSrc = m_pBits;
    int componentsPerPixel = getBytesPerPixel();
    for (int y = 0; y < getSize().y; ++y) {
        switch(m_PF) {
            case R8G8B8X8:
            case B8G8R8X8:
                {
                    componentsPerPixel = 3;
                    Pixel32 * pSrcPixel = (Pixel32 *)pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        sum += sqr(pSrcPixel->getR()-average);
                        sum += sqr(pSrcPixel->getG()-average);
                        sum += sqr(pSrcPixel->getB()-average);
                        pSrcPixel++;
                    }
                }
                break;
            case R8G8B8A8:
            case B8G8R8A8:
                {
                    componentsPerPixel = 4;
                    Pixel32 * pSrcPixel = (Pixel32 *)pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        int a = pSrcPixel->getA();
                        if (a > 0) {
                            sum += sqr((pSrcPixel->getR()*a)/255-average);
                            sum += sqr((pSrcPixel->getG()*a)/255-average);
                            sum += sqr((pSrcPixel->getB()*a)/255-average);
                            sum += sqr(pSrcPixel->getA()-average);
                        }
                        pSrcPixel++;
                    }
                }
                break;
            case I16:
                {
                    componentsPerPixel = 1;
                    unsigned short * pSrcPixel = (unsigned short *)pSrc;
                    for (int x = 0; x < m_Size.x; ++x) {
                        sum += sqr(*pSrcPixel-average);
                        pSrcPixel++;
                    }
                }
                break;
            default:
                {
                    uint8_t * pSrcComponent = pSrc;
                    for (int x = 0; x < getLineLen(); ++x) {
                        sum += sqr(*pSrcComponent-average);
                        pSrcComponent++;
                    }
                }
        }
        pSrc += m_Stride;
    }
    sum /= componentsPerPixel;
    sum /= (getSize().x*getSize().y);
    return sqrt(sum);
}

void Bitmap::dump(bool bDumpPixels) const
{
    cerr << "Bitmap: " << m_sName << endl;
    cerr << "  m_Size: " << m_Size.x << "x" << m_Size.y << endl;
    cerr << "  m_Stride: " << m_Stride << endl;
    cerr << "  m_PF: " << getPixelFormatString(m_PF) << endl;
    cerr << "  m_pBits: " << (void *)m_pBits << endl;
    cerr << "  m_bOwnsBits: " << m_bOwnsBits << endl;
    glm::ivec2 max;
    if (bDumpPixels) {
        max = m_Size;
    } else {
        max = glm::ivec2(16,1);
    }
    cerr << "  Pixel data: " << endl;
    for (int y = 0; y < max.y; ++y) {
        uint8_t * pLine = m_pBits+m_Stride*y;
        cerr << "    ";
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
        glm::ivec2 uvSize = m_Size/2;
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
