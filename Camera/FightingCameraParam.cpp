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

CameraParam cameraParam;

void CameraParam::LoadCameraParam()
{
    cv::FileStorage fs("CameraParam.yml", cv::FileStorage::READ);
    fs["camera_type"] >> cameraParam.camera_type;
    fs["camera_name"] >> cameraParam.camera_name;
    fs["camera_matrix"] >> cameraParam.camera_matrix;
    fs["camera_distortion"] >> cameraParam.camera_distortion;
    fs["resolution_width"] >> cameraParam.resolution_width;
    fs["resolution_height"] >> cameraParam.resolution_height;
    fs["auto_exposure"] >> cameraParam.auto_exposure;
    fs["exposure_time"] >> cameraParam.exposure_time;
    fs["frame_rate"] >> cameraParam.frame_rate;
    fs["brightness"] >> cameraParam.brightness;
    fs["auto_white_balance"] >> cameraParam.auto_white_balance;
    fs["balance_ratio_red"] >> cameraParam.balance_ratio_red;
    fs["balance_ratio_green"] >> cameraParam.balance_ratio_green;
    fs["balance_ratio_blue"] >> cameraParam.balance_ratio_blue;
    fs.release();
}
