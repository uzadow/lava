#include "TestSuite.h"
#include "Exception.h"

#include <iostream>

using namespace std;

namespace lava {

TestSuite::TestSuite(const string& sName)
    : Test(sName, 0)
{
}

void TestSuite::addTest(TestPtr pNewTest)
{
    m_Tests.push_back(pNewTest);
}

void TestSuite::runTests()
{
    cerr << string(getIndentLevel(), ' ') << "Running suite " << getName() << endl;
    for (auto& m_Test : m_Tests) {
        cerr << string(getIndentLevel(), ' ') << "  Running " << m_Test->getName() << endl;
        try {
            m_Test->runTests();
            aggregateStatistics(*m_Test);
            m_Test->printResults();
        } catch (Exception& ex) {
            cerr << string(getIndentLevel(), ' ') << ex.what() << endl;
            setFailed();
        } catch (std::exception& ex) {
            cerr << string(getIndentLevel(), ' ') << "    ---->> failed, std::exception: " << ex.what() << endl;
            setFailed();
        } catch (...) {
            cerr << string(getIndentLevel(), ' ') << "    ---->> failed, exception caught" << endl;
            setFailed();
        }
    }
    
    printResults();
}


}

