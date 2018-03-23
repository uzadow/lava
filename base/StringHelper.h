#pragma once

#include <string>
#include <sstream>
#include <vector>

#ifdef __GNUC__
#include <cxxabi.h>
#endif

namespace lava {

template<class T>
std::string toString(const T& i)
{
    std::stringstream stream;
    stream << i;
    return stream.str();
}

std::string toString(const bool& b);

template<class T>
std::string getFriendlyTypeName(const T& dummy)
{
    std::string sTypeName = typeid(T).name();
#ifdef __GNUC__
    int status;
    char* const pClearName = abi::__cxa_demangle (sTypeName.c_str(), 0, 0, &status);
    if (status == 0) {
        sTypeName = pClearName;
    }
#endif
    return sTypeName;
}


}
