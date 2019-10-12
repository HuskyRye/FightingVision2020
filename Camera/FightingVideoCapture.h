
#pragma once

#include "FightingCapture.h"

class FightingVideoCapture : public FightingCapture {
public:
    FightingVideoCapture(const std::string& filename);
    ~FightingVideoCapture() = default;

    bool init() final;
    bool read(cv::Mat& image) final;

private:
    cv::VideoCapture capture;
};
