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
        initBmp(I8);
        initBmp(R8G8B8);
        initBmp(R8G8B8A8);
        TEST(true);
    }

private:
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
        pBmp->dump(true);
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


