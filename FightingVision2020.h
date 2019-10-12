﻿/*
 *  Copyright (C) 2019 刘臣轩
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <iostream>

#include <opencv2/opencv.hpp>

#include "Camera/FightingCapture.h"
#include "Camera/FightingVideoCapture.h"
#include "Camera/FightingCameraCapture.h"
#include "Camera/FightingSimpleCapture.h"
#include "Camera/FightingCameraParam.h"

#include "SerialPort/SerialPort.h"
#include "SerialPort/Protocol.h"

#include "Armor/ArmorDetector.h"
#include "Rune/RuneDetector.h"
