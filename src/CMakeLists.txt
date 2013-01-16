cmake_minimum_required(VERSION 2.8)
project( chsemd )
find_package( OpenCV REQUIRED )
FIND_PACKAGE( Boost 1.40 COMPONENTS program_options REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable( chsemd main.cpp emd.c pointset.cpp db.cpp )
TARGET_LINK_LIBRARIES( chsemd ${Boost_LIBRARIES} )
target_link_libraries( chsemd ${OpenCV_LIBS} )
