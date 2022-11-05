cmake_minimum_required(VERSION 2.8)

project( WebcamFeed )

find_package( OpenCV REQUIRED )
include_directories( OpenCV REQUIRED )

## Executable
add_executable(read_vid_stream src/read_vid_stream.cpp)
add_executable(face_detect src/face_detect.cpp)
target_link_libraries(read_vid_stream ${OpenCV_LIBS})