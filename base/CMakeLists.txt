
add_library(base
        Bitmap.cpp
        Exception.cpp
        GLMHelper.cpp
        MathHelper.cpp
        PixelFormat.cpp
        StringHelper.cpp
        Test.cpp
        TestSuite.cpp
        stb_image.h
        stb_image_write.h)

target_link_libraries(base
        PUBLIC turbojpeg)

link_libraries(base)
add_executable(testbase testbase.cpp)

