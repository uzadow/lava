
add_library(base
    Test.cpp TestSuite.cpp Exception.cpp GLMHelper.cpp)

link_libraries(base)
add_executable(testbase testbase.cpp)

