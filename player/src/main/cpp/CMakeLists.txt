cmake_minimum_required(VERSION 3.4.1)

# 设置生成的so动态库最后输出路劲
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../jniLibs/${ANDROID_ABI})

# 设置clang编译器的编译选项
if (CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-declarations -Wmissing-prototypes -Wdocumentation -Wunreachable-code")
endif (CMAKE_COMPILER_IS_CLANG)

# 支持C++11标准和GNU扩展特性
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=gnu++11")

# 1. 定义so库和头文件所在目录，方便后面使用
set(ffmpeg_lib_dir ${CMAKE_SOURCE_DIR}/../../../libs/armeabi-v7a)
set(head_dir ${CMAKE_SOURCE_DIR}/include)
set(ffmpeghead_dir ${CMAKE_SOURCE_DIR}/include/ffmpeg)

# 2. 添加头文件目录
include_directories(${head_dir} ${ffmpeghead_dir})

# 3. 添加ffmpeg相关的so库
add_library(avutil
        SHARED
        IMPORTED)
set_target_properties(avutil
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavutil.so)

add_library(swresample
        SHARED
        IMPORTED)
set_target_properties(swresample
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswresample.so)

add_library(avcodec
        SHARED
        IMPORTED)
set_target_properties(avcodec
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavcodec.so)

add_library(avfilter
        SHARED
        IMPORTED)
set_target_properties(avfilter
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavfilter.so)

add_library(swscale
        SHARED
        IMPORTED)
set_target_properties(swscale
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libswscale.so)

add_library(avformat
        SHARED
        IMPORTED)
set_target_properties(avformat
        PROPERTIES IMPORTED_LOCATION
        ${ffmpeg_lib_dir}/libavformat.so)

# 查找代码中使用到的系统库
find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

find_package(oboe REQUIRED CONFIG)

# 配置目标so库编译信息
add_library(
        # Sets the name of the library.
        loser-player

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        loser-player.cpp
        Thread.cpp
        IObserver.cpp
        PlayData.cpp
        IDemux.cpp
        FFDemux.cpp
        IDecode.cpp
        FFDecode.cpp
        IResample.cpp
        FFResample.cpp
        IAudioPlay.cpp
        SLAudioPlay.cpp
        IVideoView.cpp
        GLVideoView.cpp
        Shader.cpp
        Texture.cpp
        EGL.cpp
        IPlayer.cpp
        IPlayerBuilder.cpp
        FFPlayerBuilder.cpp
        IPlayerProxy.cpp
        OboeAudioPlay.cpp)

# 指定编译目标库时，cmake要链接的库
target_link_libraries(loser-player

        # 4. 连接 FFmpeg 相关的库
        avutil
        swresample
        avcodec
        avfilter
        swscale
        avformat

        OpenSLES
        EGL
        GLESv2

        oboe::oboe
        android
        mediandk
        log)