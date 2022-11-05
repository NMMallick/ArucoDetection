cmake_minimum_required(VERSION 2.8)

project( ArucoBoardGeneration )

find_package( OpenCV REQUIRED )

include_directories( OpenCV REQUIRED )

add_executable(generate_board src/generate_board.cc)
target_link_libraries(generate_board ${OpenCV_LIBS})