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
        checkI8();
        initBmp(R8G8B8);
        initBmp(R8G8B8A8);
    }

private:
    void checkI8()
    {
        // Basic sanity check
        BitmapPtr pBmp = initBmp(I8);
        checkSanity(pBmp, I8, glm::ivec2(5,7), 5);
        uint8_t* pBits = pBmp->getPixels(0);
        unsigned stride = pBmp->getStride(0);
        TEST(pBits[0] == 0);
        TEST(pBits[6*stride+4] == 4);

        // Test copy
        BitmapPtr pBmpCopy(new Bitmap(*pBmp));
        checkSanity(pBmpCopy, I8, glm::ivec2(5,7), 5);
        pBits = pBmpCopy->getPixels(0);
        stride = pBmpCopy->getStride(0);
        TEST(pBits[0] == 0);
        TEST(pBits[6*stride+4] == 4);

        TEST(*pBmp == *pBmpCopy);
        
    }

    BitmapPtr initBmp(PixelFormat pf)
    {
        LAVA_ASSERT(!pixelFormatIsPlanar(pf));
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
        for (int y = 0; y < size[1]; ++y) {
            for (int x = 0; x < size[0]; ++x) {
                unsigned char * pPixel = pBmp->getPixels(0)+y*pBmp->getStride(0)+x*pBmp->getBytesPerPixel();
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
//        pBmp->dump(true);
        return pBmp;
    }

    void checkSanity(BitmapPtr pBmp, PixelFormat pf, const glm::ivec2& size, int minStride)
    {
        TEST(pBmp->getSize() == size);
        TEST(pBmp->getPixelFormat() == pf);
        TEST(pBmp->getStride(0) >= minStride);
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


