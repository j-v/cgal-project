cmake_minimum_required(VERSION 2.8)
project( chsemd )
find_package( OpenCV REQUIRED )

add_executable( chsemd main.cpp emd.cpp pointset.cpp db.cpp )
target_link_libraries( chsemd ${OpenCV_LIBS} )

add_executable( emdvis emdvis.cpp emd.cpp pointset.cpp db.cpp )
target_link_libraries( emdvis ${OpenCV_LIBS} )

add_executable( emdmatrix emdmatrix.cpp emd.cpp db.cpp pointset.cpp)
target_link_libraries( emdmatrix ${OpenCV_LIBS} )

add_executable( mdsvis mdsvis.cpp db.cpp )
target_link_libraries( mdsvis ${OpenCV_LIBS} )

add_executable( perftest perftest.cpp pointset.cpp emd.cpp)
target_link_libraries( perftest ${OpenCV_LIBS} )