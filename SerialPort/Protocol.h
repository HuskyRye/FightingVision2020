/*
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

#include "SerialPort.h"
#include "opencv2/opencv.hpp"

class Protocol {
public:
    explicit Protocol(SerialPort& serial_port);
    ~Protocol();
    void sendTarget(cv::Point3f& target);
    void receiveData();

private:
    SerialPort& serial_port_;
};

enum class EnemyColor : uchar { RED,
    BLUE };
enum class State : uchar { ARMOR_STATE,
    RUNE_STATE };

struct Target {
    float yaw;
    float pitch;
    float distance;
};

struct McuData {
    EnemyColor enemy_color;
    State state;
};

extern McuData mcu_data;
