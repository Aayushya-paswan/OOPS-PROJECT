cmake_minimum_required(VERSION 3.10)
project(AdmissionSystemGUI)

set(CMAKE_CXX_STANDARD 17)

# Find Windows libraries
find_library(COMCTL32 comctl32)
find_library(GDI32 gdi32)

add_executable(admission_gui main_ncurses.cpp functions.cpp)

target_link_libraries(admission_gui ${COMCTL32} ${GDI32})
target_compile_definitions(admission_gui PRIVATE UNICODE _UNICODE)