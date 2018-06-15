#pragma once

#include <iostream>
#include <string>
#include <memory>

namespace lava {
class Test
{
public:
    Test(const std::string& sName, unsigned indentLevel);
    virtual ~Test() = default;

    bool isOk();
    virtual void runTests() = 0;

    void test(bool b, const char * pszFile, unsigned line);
    void setFailed();

    int getNumSucceeded() const;
    int getNumFailed() const;
    const std::string& getName() const;
    unsigned getIndentLevel() const;

    void aggregateStatistics(const Test& childTest);
    virtual void printResults();

private:
    unsigned m_IndentLevel;
    unsigned m_NumSucceeded;
    unsigned m_NumFailed;
    std::string m_sName;
};

typedef std::shared_ptr<Test> TestPtr;

#define TEST_FAILED(s)                     \
    cerr << string(getIndentLevel()+6, ' ') << s << endl;  \
    test(false, __FILE__, __LINE__);

#define TEST(b)                            \
    cerr << string(getIndentLevel()+4, ' ') << "  TEST(" << #b << ")" << endl;  \
    test(b, __FILE__, __LINE__);

#define TEST_EXCEPTION(expr, ExceptionType)\
    cerr << string(getIndentLevel()+4, ' ') << "  TEST_EXCEPTION(" << #expr << ")" << endl; \
    {                                      \
        bool bOK = false;                  \
        try {                              \
            expr;                          \
        } catch (const ExceptionType&) {   \
            bOK = true;                    \
        }                                  \
        test(bOK, __FILE__, __LINE__);       \
    }

#define QUIET_TEST(b)                      \
    if(!(b)) {                             \
        cerr << string(getIndentLevel()+4, ' ') << "  TEST(" << #b << ")" << endl;  \
    }                                      \
    test(b, __FILE__, __LINE__);

#define TEST_MSG(msg)                            \
    cerr << string(getIndentLevel()+2, ' ') << msg << endl;
}

