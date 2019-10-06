#pragma once

#include <iostream>

#include <opencv2/opencv.hpp>

#include "Camera/FightingCapture.h"
#include "Camera/FightingVideoCapture.h"
#include "Camera/FightingCameraCapture.h"
#include "Camera/FightingSimpleCapture.h"

#include "SerialPort/SerialPort.h"
#include "SerialPort/Protocol.h"

#include "Armor/ArmorDetector.h"

enum class State { ENERGY_STATE, ARMOR_STATE };
