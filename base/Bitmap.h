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
    virtual ~Bitmap();

    void load(const std::string& sName);
    void save(const std::string& sName) const;
    
    glm::ivec2 getSize() const;
    glm::ivec2 getPlaneSize(unsigned i) const;
    int getStride(unsigned i) const;
    PixelFormat getPixelFormat() const;
    uint8_t* getPixels(unsigned i);
    const uint8_t* getPixels(unsigned i) const;
    int getBytesPerPixel() const;
    int getLineLen() const;
    int getMemNeeded() const;

    // TODO: operator -
    // BitmapPtr subtract(const Bitmap& pOtherBmp);
    float getAvg() const;
    float getStdDev() const;

    bool operator ==(const Bitmap& otherBmp);
    void dump(bool bDumpPixels=false) const;

    static int getPreferredStride(int width, PixelFormat pf);

private:
    void initWithData(const uint8_t* pBits, int stride);
    void initWithData(const std::vector<uint8_t *>& pPlanes, const std::vector<int>& strides);
    void allocBits();
    void checkValidSize() const;

    const std::vector<int>& getStrides() const;
    const std::vector<uint8_t*>& getPlanes() const;

    glm::ivec2 m_Size;
    PixelFormat m_PF;
    std::vector<int> m_Strides;
    std::vector<uint8_t*> m_pPlanes; 
};

}
