#pragma once

#include <string>
#include <stdexcept>


namespace lava {

class Exception: public std::runtime_error
{
    public:
        explicit Exception(const std::string& sErr = "");
};

void debugBreak();
void lavaAssert(bool b, const char * pszFile, int line, const char * pszReason=nullptr);

#define LAVA_ASSERT(b) lavaAssert((b) != 0, __FILE__, __LINE__);
#define LAVA_ASSERT_MSG(b, pszReason) lavaAssert((b) != 0, __FILE__, __LINE__, pszReason);

}

