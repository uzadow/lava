#pragma once

#include "PixelFormat.h"

#include "Rect.h"
#include "GLMHelper.h"

#include <boost/shared_ptr.hpp>

#include <vector>
#include <iostream>

namespace lava {

class Bitmap;
typedef boost::shared_ptr<Bitmap> BitmapPtr;

class Bitmap
{
public:
    Bitmap(glm::vec2 size, PixelFormat pf);
    Bitmap(IntPoint size, PixelFormat pf, const UTF8String& sName="", int stride=0);
    Bitmap(IntPoint size, PixelFormat pf, unsigned char * pBits, 
            int stride, bool bCopyBits, const UTF8String& sName="");
    Bitmap(const Bitmap& origBmp);
    Bitmap(const Bitmap& origBmp, bool bOwnsBits);
    Bitmap(Bitmap& origBmp, const IntRect& rect);
    virtual ~Bitmap();

    Bitmap &operator =(const Bitmap & origBmp);
    
    // Does pixel format conversion if nessesary.
    void copyPixels(const Bitmap& origBmp);
    void copyYUVPixels(const Bitmap& yBmp, const Bitmap& uBmp, const Bitmap& vBmp,
            bool bJPEG);
    void save(const UTF8String& sName);
    
    IntPoint getSize() const;
    int getStride() const;
    PixelFormat getPixelFormat() const;
    void setPixelFormat(PixelFormat pf);
    unsigned char* getPixels();
    const unsigned char* getPixels() const;
    void setPixels(const unsigned char* pPixels, int stride=0);
    bool ownsBits() const;
    const std::string& getName() const;
    int getBytesPerPixel() const;
    int getLineLen() const;
    int getMemNeeded() const;
    bool hasAlpha() const;
    HistogramPtr getHistogram(int stride = 1) const;
    void getMinMax(int stride, int& min, int& max) const;
    void setAlpha(const Bitmap& alphaBmp);

    Pixel32 getPythonPixel(const glm::vec2& pos);
    template<class PIXEL>
    void setPixel(const IntPoint& p, PIXEL color);

    BitmapPtr subtract(const Bitmap& pOtherBmp);
    void blt(const Bitmap& otherBmp, const IntPoint& pos);
    float getAvg() const;
    float getChannelAvg(int channel) const;
    float getStdDev() const;

    bool operator ==(const Bitmap& otherBmp);
    void dump(bool bDumpPixels=false) const;

    static int getPreferredStride(int width, PixelFormat pf);

private:
    void initWithData(unsigned char* pBits, int stride, bool bCopyBits);
    void allocBits(int stride=0);
    void YCbCrtoBGR(const Bitmap& origBmp);
    void YCbCrtoI8(const Bitmap& origBmp);
    void I8toI16(const Bitmap& origBmp);
    void I8toRGB(const Bitmap& origBmp);
    void I16toI8(const Bitmap& origBmp);
    void BGRtoB5G6R5(const Bitmap& origBmp);
    void ByteRGBAtoFloatRGBA(const Bitmap& origBmp);
    void FloatRGBAtoByteRGBA(const Bitmap& origBmp);
    void BY8toRGBNearest(const Bitmap& origBmp);
    void BY8toRGBBilinear(const Bitmap& origBmp);

    IntPoint m_Size;
    int m_Stride;
    PixelFormat m_PF;
    unsigned char* m_pBits;
    bool m_bOwnsBits;
    UTF8String m_sName;

    static bool s_bMagickInitialized;
    static bool s_bGTKInitialized;
};

BitmapPtr YCbCr2RGBBitmap(BitmapPtr pYBmp, BitmapPtr pUBmp, BitmapPtr pVBmp);

template<class PIXEL>
void Bitmap::setPixel(const IntPoint& p, PIXEL color)
{
    *(PIXEL*)(&(m_pBits[p.y*m_Stride+p.x*getBytesPerPixel()])) = color;
}

}
