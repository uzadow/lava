#include "TestSuite.h"

#include "Exception.h"
#include "Bitmap.h"

#include <stdio.h>

using namespace std;

namespace lava {

class BmpTest: public Test
{
public:
    BmpTest()
      : Test("BmpTest", 2)
    {
    }

    void runTests()
    {
        checkOnePF(I8, glm::ivec2(5,7), 0, 4);
        checkOnePF(R8G8B8, glm::ivec2(5,7), 0, 4*16);
        checkOnePF(R8G8B8A8, glm::ivec2(5,7), 0, 0x80);
        checkOnePF(YCbCr420p, glm::ivec2(6,8), 0, 5);
    }

private:
    void checkOnePF(PixelFormat pf, const glm::ivec2& size, uint8_t firstByte, uint8_t lastByte)
    {
        TEST_MSG(pf);
        // Basic sanity check
        BitmapPtr pBmp = initBmp(pf);
        checkSanity(pBmp, pf, size, 5, firstByte, lastByte);

        // Copy, equals
        BitmapPtr pBmpCopy(new Bitmap(*pBmp));
        checkSanity(pBmpCopy, pf, size, 5, firstByte, lastByte);
        TEST(*pBmp == *pBmpCopy);

        // Subtract, avg, stdev
        BitmapPtr pDiffBmp = pBmp->subtract(*pBmpCopy);
        checkSanity(pDiffBmp, pf, size, 5, 0, 0);
        TEST(pDiffBmp->getAvg() == 0);
        TEST(pDiffBmp->getStdev() == 0);
    }

    void checkSanity(BitmapPtr pBmp, PixelFormat pf, const glm::ivec2& size, int minStride,
            uint8_t firstByte, uint8_t lastByte)
    {
        TEST(pBmp->getSize() == size);
        TEST(pBmp->getPixelFormat() == pf);
        uint8_t* pBits = pBmp->getPixels(0);
        int stride = pBmp->getStride(0);
        TEST(stride >= minStride);

        TEST(pBits[0] == firstByte);
        uint8_t bpp = getBytesPerPixel(pf);
        unsigned offset = (size.y-1)*stride + (size.x)*bpp - 1;
        TEST(pBits[offset] == lastByte);
    }

    BitmapPtr initBmp(PixelFormat pf)
    {
        glm::ivec2 size;
        switch(pf) {
            case YCbCr422:
                size = glm::ivec2(6, 7);
                break;
            case YCbCr420p:
            case YCbCrJ420p:
            case YCbCrA420p:
                size = glm::ivec2(6, 8);
                break;
            default:
                size = glm::ivec2(5, 7);
        }
        BitmapPtr pBmp(new Bitmap(size, pf));
        int bpp = pBmp->getBytesPerPixel();
        for (unsigned p = 0; p < getNumPixelFormatPlanes(pf); ++p) {
            glm::ivec2 planeSize = pBmp->getPlaneSize(p);
            for (int y = 0; y < planeSize.y; ++y) {
                for (int x = 0; x < planeSize.x; ++x) {
                    unsigned char * pPixel = pBmp->getPixels(p) + y*pBmp->getStride(p) + x*pBmp->getBytesPerPixel();
                    *(pPixel) = x;
                    if (bpp > 1) {
                        *(pPixel+1) = 0;
                    }
                    if (bpp > 2) {
                        *(pPixel+2) = x*16;
                        *(pPixel+1) = 16*y;
                    }
                    if (bpp > 3) {
                        *(pPixel+3) = 0x80;
                    }
                }
            }
        }
        return pBmp;
    }
};

class BaseTestSuite: public TestSuite
{
public:
    BaseTestSuite()
        : TestSuite("BaseTestSuite")
    {
        addTest(TestPtr(new BmpTest));
    }
};

}

int main(int nargs, char** args)
{
    lava::BaseTestSuite suite;
    suite.runTests();
    bool bOK = suite.isOk();

    if (bOK) {
        return 0;
    } else {
        return 1;
    }
}


