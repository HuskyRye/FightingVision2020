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

#include "opencv2/opencv.hpp"
#include "FightingParam.h"

struct CameraParam : FightingParam{
    std::string camera_type;
    std::string camera_name;

    cv::Mat camera_matrix;
    cv::Mat camera_distortion;

    int resolution_width;
    int resolution_height;

    bool auto_exposure;
    double exposure_time;
    double frame_rate;
    int brightness;

    bool auto_white_balance;
    double balance_ratio_red;
    double balance_ratio_green;
    double balance_ratio_blue;

    void LoadParam() final;
};

extern CameraParam cameraParam;
