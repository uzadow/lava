#pragma once

#define GLM_FORCE_RADIANS
#include "../glm/fwd.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtx/io.hpp"

#include <iostream>
#include <vector>

namespace lava {

template<typename NUM, glm::precision precision>
bool almostEqual(const glm::detail::tvec2<NUM, precision>& v1,
                 const glm::detail::tvec2<NUM, precision>& v2);
template<typename NUM, glm::precision precision>
bool almostEqual(const glm::detail::tvec4<NUM, precision>& v1,
                 const glm::detail::tvec4<NUM, precision>& v2);

}
