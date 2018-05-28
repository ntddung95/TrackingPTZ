cmake_minimum_required(VERSION 2.8.12)
project(Tracking)
find_package( dlib REQUIRED )
find_package( OpenCV REQUIRED )

include_directories(include)

include_directories(${CMAKE_SOURCE_DIR}/src_kcf/ ${CMAKE_SOURCE_DIR}/src_kcf/piotr_fhog ${CMAKE_SOURCE_DIR}/src_kcf/cn)
add_subdirectory(src_kcf)

add_executable( kcf_main src/main_vot.cpp)
target_link_libraries( kcf_main curl ${dlib_LIBS} ${OpenCV_LIBS} pthread ${CMAKE_THREAD_LIBS_INIT} kcf)

