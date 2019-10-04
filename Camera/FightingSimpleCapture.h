#pragma once

#include "FightingCapture.h"

class FightingSimpleCapture : public FightingCapture {
public:
    FightingSimpleCapture(int camera_id);
    ~FightingSimpleCapture() = default;

    bool init() final;
    bool read(cv::Mat& image) final;

private:
    cv::VideoCapture capture;
};