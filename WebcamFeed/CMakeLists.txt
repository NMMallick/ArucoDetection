cmake_minimum_required(VERSION 2.8)

project( WebcamFeed )

find_package( OpenCV REQUIRED )
include_directories( OpenCV REQUIRED )

## Executable
add_executable(read_vid_stream src/read_vid_stream.cc)

target_link_libraries(read_vid_stream ${OpenCV_LIBS})