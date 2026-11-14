cmake_minimum_required(VERSION 3.23)

project(GifII)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/GIFII_TEMPLATE_DISK.po ${CMAKE_CURRENT_BINARY_DIR}/GIFII_TEMPLATE_DISK.po COPYONLY)

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
    lib/Cadius/Dc_Prodos.h
    lib/Cadius/Dc_Prodos.c
    lib/Cadius/Dc_Shared.c
    lib/Cadius/Dc_Shared.h
    lib/Cadius/Dc_Memory.c
    lib/Cadius/Prodos_Add.c
    lib/Cadius/Prodos_Create.c
    lib/Cadius/Dc_OS.c
)