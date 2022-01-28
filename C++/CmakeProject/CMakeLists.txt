project(ANMS_Codes)
cmake_minimum_required(VERSION 2.8.3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

find_package( OpenCV REQUIRED )
if( OpenCV_FOUND )
list( APPEND ThirdParty_LIBS ${OpenCV_LIBS} )
    include_directories( ${OpenCV_INCLUDE_DIRS} )
endif( OpenCV_FOUND )

include_directories(
    /usr/include/opencv
)

add_executable( ANMS_Codes main.cpp 
		../include/nanoflann.hpp
		../include/range-tree/lrtypes.h
		../include/range-tree/ranget.h
		../include/anms.h)

target_link_libraries( ANMS_Codes ${OpenCV_LIBS} )
