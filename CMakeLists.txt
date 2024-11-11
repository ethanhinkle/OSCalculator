cmake_minimum_required(VERSION 3.12)

project(app)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

include_directories(
    "${PROJECT_SOURCE_DIR}/include/"
    "${PROJECT_SOURCE_DIR}/include/curl/curl"
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(BUILD_PLATFORM STREQUAL "windows_mingw")
    add_definitions(-DUNICODE -D_UNICODE)
    target_link_libraries(${PROJECT_NAME} -static -static-libgcc -static-libstdc++)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/libs/curl/libcurl.dll.a)

endif()