#ifndef _Exception_H_
#define _Exception_H_

#include <string>
#include <exception>


namespace lava {

class Exception: public std::exception
{
    public:
        Exception(const std::string& sErr = "");
        Exception(const Exception& ex);
        virtual ~Exception() throw();
        virtual const std::string& getStr() const;
        virtual const char* what() const throw();

    private:
        std::string m_sErr;
};

#ifdef _WIN32
    #pragma warning(pop)
#endif

void debugBreak();
void lavaAssert(bool b, const char * pszFile, int line, const char * pszReason=0);

#define LAVA_ASSERT(b) lavaAssert((b) != 0, __FILE__, __LINE__);
#define LAVA_ASSERT_MSG(b, pszReason) lavaAssert((b) != 0, __FILE__, __LINE__, pszReason);

}

#endif
