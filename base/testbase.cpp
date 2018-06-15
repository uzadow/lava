#include "TestSuite.h"

#include "Exception.h"
#include "Bitmap.h"
#include "Rect.h"

#include <cstdio>

using namespace std;

const std::string sMediaDir = "../../testmedia";

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
        Bitmap::setEpsilon(0.5, 2);

        checkOnePF(I8, glm::ivec2(5,7), 0, 4);
        checkOnePF(R8G8B8, glm::ivec2(5,7), 0, 4*16);
        checkOnePF(R8G8B8A8, glm::ivec2(5,7), 0, 0x80);
        checkOnePF(YCbCr420p, glm::ivec2(6,8), 0, 5);

        checkLoadSave();
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

    void checkLoadSave()
    {
        TEST_MSG("Loader");
        Bitmap bmp1 = Bitmap::load(sMediaDir + "/invaders.png");
        bmp1.saveToPNG(sMediaDir + "/save_invaders1.png");
        TEST(bmp1.getPixelFormat() == R8G8B8);
        Bitmap bmp2 = Bitmap::load(sMediaDir + "/invaders.jpg");
        TEST(bmp2.getPixelFormat() == YCbCrJ420p);
        Bitmap bmp3 = bmp2.createStdBmp(); // Convert to RGB
        bmp3.saveToPNG(sMediaDir + "/save_invaders3.png");
        TEST(Bitmap::almostEqual(bmp1, bmp3));

        TEST_EXCEPTION(Bitmap::load("FileDoesntExist.png"), Exception);
    }

    void checkSanity(const BitmapPtr& pBmp, PixelFormat pf, const glm::ivec2& size, int minStride,
            uint8_t firstByte, uint8_t lastByte)
    {
        TEST(pBmp->getSize() == size);
        TEST(pBmp->getPixelFormat() == pf);
        uint8_t* pBits = pBmp->getPixels(0);
        int stride = pBmp->getStride(0);
        TEST(stride >= minStride);

        TEST(pBits[0] == firstByte);
        unsigned bpp = getBytesPerPixel(pf);
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
                    uint8_t * pPixel = pBmp->getPixels(p) + y*pBmp->getStride(p) + x*pBmp->getBytesPerPixel();
                    *(pPixel) = uint8_t(x);
                    if (bpp > 1) {
                        *(pPixel+1) = 0;
                    }
                    if (bpp > 2) {
                        *(pPixel+2) = uint8_t(x*16);
                        *(pPixel+1) = uint8_t(16*y);
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


