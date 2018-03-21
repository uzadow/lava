#pragma once

#include "Test.h"

#include <vector>

namespace lava {
class TestSuite: public Test
{
public:
    TestSuite(const std::string& sName);
    virtual ~TestSuite();

    void addTest(TestPtr pNewTest);

    virtual void runTests();

private:
    std::vector<TestPtr> m_Tests;
};
    
}
