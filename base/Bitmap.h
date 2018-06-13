#pragma once

#include "PixelFormat.h"

#include "Rect.h"
#include "GLMHelper.h"

#include <boost/shared_ptr.hpp>

#include <vector>
#include <iostream>
#include <string>

namespace lava {

class Bitmap;
typedef boost::shared_ptr<Bitmap> BitmapPtr;

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
    void save(const std::string& sName) const;

    glm::ivec2 getSize() const;
    glm::ivec2 getPlaneSize(unsigned i) const;
    int getStride(unsigned i) const;
    PixelFormat getPixelFormat() const;
    uint8_t* getPixels(unsigned i);
    const uint8_t* getPixels(unsigned i) const;
    int getBytesPerPixel() const;
    int getLineLen(unsigned i) const;
    int getMemNeeded() const;

    // TODO: operator -
    BitmapPtr subtract(const Bitmap& otherBmp);
    float getAvg() const;
    float getStdev() const;

    bool operator ==(const Bitmap& otherBmp);
    void dump(bool bDumpPixels=false) const;

    static int getPreferredStride(int width, PixelFormat pf);

    static void setEpsilon(float maxAvgDiff, float maxStdevDiff);
    static bool almostEqual(const Bitmap& bmp1, const Bitmap& bmp2);

private:
    void initWithData(const uint8_t* pBits, int stride);
    void initWithData(const std::vector<uint8_t *>& pPlanes, const std::vector<int>& strides);
    void initPlaneWithData(unsigned i, const uint8_t* pBits, int stride);
    void allocBits();
    void checkValidSize() const;

    glm::ivec2 m_Size;
    PixelFormat m_PF;
    std::vector<int> m_Strides;
    std::vector<uint8_t*> m_pPlanes;

    static float s_MaxAvgDiff;
    static float s_MaxStdevDiff;
};

}
