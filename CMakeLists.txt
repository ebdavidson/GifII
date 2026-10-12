cmake_minimum_required(VERSION 3.23)

project(GifII)

include_directories("lib")

add_executable(GifII)

target_sources(GifII
    PRIVATE
    src/main.cpp
    src/GifII.cpp
    src/GifII.h
    lib/giflib/gif_lib.h
    lib/giflib/dgif_lib.c
    lib/giflib/gifalloc.c
    lib/giflib/openbsd-reallocarray.c
    lib/EasyGifReader/EasyGifReader.h
    lib/EasyGifReader/EasyGifReader.cpp
)