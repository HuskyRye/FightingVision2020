
#pragma once

#include "opencv2/opencv.hpp"

struct CameraInfo {
    std::string camera_name;

    cv::Mat camera_matrix;
    cv::Mat camera_distortion;

    int resolution_width;
    int resolution_height;

    bool auto_exposure;
    // double exposure_value; // ???
    double exposure_time;
    double frame_rate;
    int brightness;
    double gamma;

    bool auto_white_balance; // TODO: config this in FightingCameraCapture.cpp
    double balance_ratio_red;
    double balance_ratio_green;
    double balance_ratio_blue;
};

extern CameraInfo cameraInfo;
void LoadCameraParam();
