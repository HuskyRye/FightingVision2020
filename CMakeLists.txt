# CMakeList.txt: FightingVision2020 的 CMake 项目，在此处包括源代码并定义项目特定的逻辑。

cmake_minimum_required (VERSION 3.5)

project(FightingVision2020)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D${CMAKE_SYSTEM_NAME}")

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(OpenCV_DIR C:/opencv/build)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
endif()

find_package(OpenCV REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/Camera/Include)

set(SRC
    "FightingVision2020.cpp"
    "SerialPort/SerialPort.cpp"
    "SerialPort/Protocol.cpp"
	"Camera/FightingMVCapture.cpp"
    "Camera/FightingVideoCapture.cpp"
    "Camera/FightingUSBCapture.cpp"
    "Armor/ArmorDetector.cpp"
    "Rune/RuneDetector.cpp"
    "Config/CameraParam.cpp"
    "Config/ArmorParam.cpp"
    "Config/RuneParam.cpp"
)

add_executable (FightingVision2020 ${SRC})

target_link_libraries( FightingVision2020 ${OpenCV_LIBS} )

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries( FightingVision2020 "${PROJECT_SOURCE_DIR}/Camera/MVCAMSDK_X64.lib" )
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries( FightingVision2020 "${PROJECT_SOURCE_DIR}/Camera/libMVSDK.so" )
endif()

file(COPY "${PROJECT_SOURCE_DIR}/Config/CameraParam.yml" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY "${PROJECT_SOURCE_DIR}/Config/ArmorParam.yml" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY "${PROJECT_SOURCE_DIR}/Config/RuneParam.yml" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY "${PROJECT_SOURCE_DIR}/Template/" DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
