#pragma once

#include "opencv2/opencv.hpp"

class RuneDetector {
public:
    RuneDetector();
    ~RuneDetector();
    bool DetectRune(cv::Mat& src, cv::Point3f& target_3d);

private:
};