
#pragma once

#include <opencv2/opencv.hpp>

class FightingCapture {
public:
    virtual bool init() = 0;
    virtual bool read(cv::Mat&) = 0;
    virtual ~FightingCapture() = default;
};
