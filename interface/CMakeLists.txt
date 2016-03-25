CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT( Tianchi )

SET( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
SET( CMAKE_CXX_FLAGS "-std=c++11")

INCLUDE_DIRECTORIES(/usr/local/include/opencv /usr/local/include ${PROJECT_SOURCE_DIR}/include)
LINK_DIRECTORIES(/usr/local/lib ${PROJECT_SOURCE_DIR}/lib)

ADD_LIBRARY(alisc_intf SHARED ${PROJECT_SOURCE_DIR}/src/alisc_intf.cpp)

ADD_EXECUTABLE( train ${PROJECT_SOURCE_DIR}/src/Train.cpp)
TARGET_LINK_LIBRARIES(train libopencv_core.so libopencv_imgproc.so libopencv_features2d.so
    libopencv_highgui.so libopencv_nonfree.so)

ADD_EXECUTABLE( test ${PROJECT_SOURCE_DIR}/src/Test.cpp)
TARGET_LINK_LIBRARIES(test libalisc_intf.so libopencv_core.so libopencv_imgproc.so libopencv_features2d.so
    libopencv_highgui.so libopencv_nonfree.so)

ADD_EXECUTABLE( search ${PROJECT_SOURCE_DIR}/src/Search.cpp)
TARGET_LINK_LIBRARIES(search libalisc_intf.so libopencv_core.so libopencv_imgproc.so libopencv_features2d.so
    libopencv_highgui.so libopencv_nonfree.so)

ADD_EXECUTABLE( evaluation ${PROJECT_SOURCE_DIR}/src/Evaluation.cpp)
