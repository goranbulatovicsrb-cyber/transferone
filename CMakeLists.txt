cmake_minimum_required(VERSION 3.16)
project(TransferOne)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

add_executable(TransferOne
    main.cpp
    MainWindow.cpp
    MainWindow.h
    CopyWorker.cpp
    CopyWorker.h
)

target_link_libraries(TransferOne PRIVATE Qt6::Widgets)
