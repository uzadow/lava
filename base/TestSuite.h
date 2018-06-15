#pragma once

#include "Test.h"

#include <vector>

namespace lava {
class TestSuite: public Test
{
public:
    explicit TestSuite(const std::string& sName);
    virtual ~TestSuite() = default;

    void addTest(TestPtr pNewTest);

    virtual void runTests();

private:
    std::vector<TestPtr> m_Tests;
};
    
}

