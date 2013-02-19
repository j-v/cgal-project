cmake_minimum_required(VERSION 2.8)
project( chsemd )
find_package( OpenCV REQUIRED )

add_executable( chsemd main.cpp emd.cpp pointset.cpp db.cpp )
target_link_libraries( chsemd ${OpenCV_LIBS} )
