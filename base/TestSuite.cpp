#include "TestSuite.h"
#include "Exception.h"

#include <iostream>

using namespace std;

namespace lava {

TestSuite::TestSuite(const string& sName)
    : Test(sName, 0)
{
}

TestSuite::~TestSuite()
{
}

void TestSuite::addTest(TestPtr pNewTest)
{
    m_Tests.push_back(pNewTest);
}

void TestSuite::runTests()
{
    cerr << string(getIndentLevel(), ' ') << "Running suite " << getName() << endl;
    for (unsigned i = 0; i < m_Tests.size(); ++i) {
        cerr << string(getIndentLevel(), ' ') << "  Running " << m_Tests[i]->getName() << endl;
        try {
            m_Tests[i]->runTests();
            aggregateStatistics(*m_Tests[i]);
            m_Tests[i]->printResults();
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

