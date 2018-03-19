#include "GLMHelper.h"

#include "../glm/gtx/rotate_vector.hpp"

using namespace std;

namespace lava {

template<typename NUM, glm::precision precision>
bool almostEqual(const glm::detail::tvec2<NUM, precision>& v1,
                 const glm::detail::tvec2<NUM, precision>& v2)
{
    return (fabs(v1.x-v2.x)+fabs(v1.y-v2.y)) < 0.0001;
}

template<typename NUM, glm::precision precision>
bool almostEqual(const glm::detail::tvec4<NUM, precision>& v1,
                 const glm::detail::tvec4<NUM, precision>& v2)
{
    return (fabs(v1.x-v2.x)+fabs(v1.y-v2.y)+fabs(v1.z-v2.z)+fabs(v1.w-v2.w)) < 0.0001;
}

}

