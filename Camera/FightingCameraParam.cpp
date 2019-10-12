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

#include "FightingCameraParam.h"

CameraInfo cameraInfo;

void LoadCameraParam()
{
    cv::FileStorage fs("DahuaCamera.yml", cv::FileStorage::READ);
    fs["camera_name"] >> cameraInfo.camera_name;
    fs["camera_matrix"] >> cameraInfo.camera_matrix;
    fs["camera_distortion"] >> cameraInfo.camera_distortion;
    fs["resolution_width"] >> cameraInfo.resolution_width;
    fs["resolution_height"] >> cameraInfo.resolution_height;
    fs["auto_exposure"] >> cameraInfo.auto_exposure;
    fs["exposure_time"] >> cameraInfo.exposure_time;
    fs["frame_rate"] >> cameraInfo.frame_rate;
    fs["brightness"] >> cameraInfo.brightness;
    fs["gamma"] >> cameraInfo.gamma;
    fs["auto_white_balance"] >> cameraInfo.auto_white_balance;
    fs["balance_ratio_red"] >> cameraInfo.balance_ratio_red;
    fs["balance_ratio_green"] >> cameraInfo.balance_ratio_green;
    fs["balance_ratio_blue"] >> cameraInfo.balance_ratio_blue;
    fs.release();
}
