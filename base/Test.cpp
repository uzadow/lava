#include "Test.h"

using namespace std;

namespace lava {

Test::Test(const string & sName, int indentLevel)
    : m_IndentLevel(indentLevel),
      m_NumSucceeded(0),
      m_NumFailed(0),
      m_sName(sName)
{
}

Test::~Test()
{
}

void Test::test(bool b, const char * pszFile, int line)
{
    if (b) {
        m_NumSucceeded++;
    } else {
        cerr << string(m_IndentLevel, ' ') << "    ---->> failed at " << pszFile
                << ", " << line << endl;
        m_NumFailed++;
    }
}


bool Test::isOk()
{
    return m_NumFailed == 0;
}

void Test::setFailed()
{
    m_NumFailed++;
}

int Test::getNumSucceeded() const
{
    return m_NumSucceeded;
}

int Test::getNumFailed() const
{
    return m_NumFailed;
}

int Test::getIndentLevel() const
{
    return m_IndentLevel;
}

const std::string& Test::getName() const
{
    return m_sName;
}

void Test::aggregateStatistics(const Test& ChildTest)
{
    m_NumSucceeded += ChildTest.getNumSucceeded();
    m_NumFailed += ChildTest.getNumFailed();
}

void Test::printResults()
{
    if (m_NumFailed == 0) {
        cerr << string(m_IndentLevel, ' ') << m_sName << " succeeded." << endl;
    } else {
        cerr << string(m_IndentLevel, ' ') << "######## " << m_sName << 
            " failed. ########" << endl;
    }
        
}
    
}

