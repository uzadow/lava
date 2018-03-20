
add_library(base
    Test.cpp TestSuite.cpp Exception.cpp GLMHelper.cpp Bitmap.cpp PixelFormat.cpp)

link_libraries(base)
add_executable(testbase testbase.cpp)

