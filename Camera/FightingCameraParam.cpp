
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
