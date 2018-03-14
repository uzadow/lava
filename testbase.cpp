#include "TestSuite.h"

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
        TEST(true);
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


