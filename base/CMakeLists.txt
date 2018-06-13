
add_library(base
    Bitmap.cpp
    Exception.cpp
    GLMHelper.cpp
    MathHelper.cpp
    PixelFormat.cpp
    StringHelper.cpp
    Test.cpp 
    TestSuite.cpp stb_image.h)

link_libraries(base)
add_executable(testbase testbase.cpp)

