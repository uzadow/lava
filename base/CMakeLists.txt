
add_library(base
    Bitmap.cpp
    Exception.cpp
    GLMHelper.cpp
    MathHelper.cpp
    PixelFormat.cpp
    Test.cpp 
    TestSuite.cpp)

link_libraries(base)
add_executable(testbase testbase.cpp)

