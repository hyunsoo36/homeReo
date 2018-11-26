cmake_minimum_required (VERSION 2.8) 


set(CMAKE_MODULE_PATH "/usr/local/lib/cmake/${CMAKE_MODULE_PATH}") 
find_package(raspicam REQUIRED)
find_package(OpenCV)
find_package(Threads)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


include_directories (include)

set(src_files
    ./src/Util.cpp
    ./src/ImageProcessing.cpp
    ./src/CoolingFanControl.cpp
    )

find_library(wiringPi_LIB wiringPi)

add_compile_options()

add_executable (homeReo Main.cpp ${src_files})  

target_link_libraries (homeReo ${raspicam_CV_LIBS} ${wiringPi_LIB} Threads::Threads )

