cmake_minimum_required(VERSION 2.8)

project( ArucoBoardGeneration )

find_package( OpenCV REQUIRED )

include_directories( OpenCV REQUIRED )
include_directories(include)

add_executable(generate_board src/GenerateCharucoBoard.cc)
add_executable(detect_tags src/DetectTags.cc)
add_executable(calibrate_cam src/CalibrateCamera.cc)

target_link_libraries(generate_board ${OpenCV_LIBS})
target_link_libraries(detect_tags ${OpenCV_LIBS})
target_link_libraries(calibrate_cam ${OpenCV_LIBS})