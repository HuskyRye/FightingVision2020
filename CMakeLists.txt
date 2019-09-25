# CMakeList.txt: FightingVision2020 的 CMake 项目，在此处包括源代码并定义项目特定的逻辑。

cmake_minimum_required (VERSION 3.8)

find_package(OpenCV REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/Camera/Include)

SET(SRC
    "FightingVision2020.cpp"
    "FightingVision2020.h"
    "Camera/FightingVideoCapture.cpp"
    "Camera/FightingCameraCapture.cpp"
)

add_executable (FightingVision2020 ${SRC})

target_link_libraries( FightingVision2020 ${OpenCV_LIBS} )

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries( FightingVision2020 "${PROJECT_SOURCE_DIR}/Camera/MVSDKmd.lib" )
    target_link_libraries( FightingVision2020 "${PROJECT_SOURCE_DIR}/Camera/ImageConvert.lib" )
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries( FightingVision2020 "${PROJECT_SOURCE_DIR}/Camera/libMVSDK.so" )
    target_link_libraries( FightingVision2020 "${PROJECT_SOURCE_DIR}/Camera/libImageConvert.so" )
endif()

