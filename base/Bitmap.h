#pragma once

#include "PixelFormat.h"

#include "Rect.h"
#include "GLMHelper.h"

#include <turbojpeg.h>

#include <memory>
#include <vector>
#include <iostream>
#include <string>

namespace lava {

class Bitmap;
typedef std::shared_ptr<Bitmap> BitmapPtr;

class Bitmap
{
public:
    Bitmap(glm::vec2 size, PixelFormat pf);
    Bitmap(glm::ivec2 size, PixelFormat pf);
    Bitmap(glm::ivec2 size, PixelFormat pf, uint8_t* pBits, int stride);
    Bitmap(glm::ivec2 size, PixelFormat pf, const std::vector<uint8_t*>& pPlanes,
            const std::vector<int>& strides);
    Bitmap(const Bitmap& origBmp);
    Bitmap(Bitmap&& origBmp) = default;
    virtual ~Bitmap();

    static Bitmap load(const std::string& sName);
    void saveToPNG(const std::string& sName) const;

    glm::ivec2 getSize() const;
    glm::ivec2 getPlaneSize(unsigned i) const;
    int getStride(unsigned i) const;
    PixelFormat getPixelFormat() const;
    uint8_t* getPixels(unsigned i);
    const uint8_t* getPixels(unsigned i) const;
    int getBytesPerPixel() const;
    int getLineLen(unsigned i) const;

    // TODO: operator -
    BitmapPtr subtract(const Bitmap& otherBmp);
    float getAvg() const;
    float getStdev() const;

    bool operator ==(const Bitmap& otherBmp);
    void dump(bool bDumpPixels=false) const;

    // Returns a copy of this Bitmap with Pixelformat I8, R8G8B8, or R8G8B8A8.
    // Runs conversion if necessary.
    Bitmap createStdBmp() const;

    static int getPreferredStride(int width, PixelFormat pf);

    static void setEpsilon(float maxAvgDiff, float maxStdevDiff);
    static bool almostEqual(const Bitmap& bmp1, const Bitmap& bmp2);

private:
    void initWithData(const uint8_t* pBits, int stride);
    void initWithData(const std::vector<uint8_t *>& pPlanes, const std::vector<int>& strides);
    void initPlaneWithData(unsigned i, const uint8_t* pBits, int stride);
    void allocBits();
    void allocPlane(const glm::ivec2& size);
    Bitmap createRGBFromYUVPlanarBmp() const;

    glm::ivec2 m_Size;
    PixelFormat m_PF;
    std::vector<int> m_Strides;
    std::vector<uint8_t*> m_pPlanes;

    static float s_MaxAvgDiff;
    static float s_MaxStdevDiff;
    static tjhandle s_JPEGDecompressor;
};

}
