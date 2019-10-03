#pragma once

#include "FightingCapture.h"

#include "CircularQueue.h"

#include "GenICam/Camera.h"
#include "GenICam/StreamSource.h"
#include "GenICam/System.h"
#include "ImageConvert.h"

using namespace Dahua::GenICam;

class FightingCameraCapture : public FightingCapture {
public:
    FightingCameraCapture();
    ~FightingCameraCapture();

    bool init() final;
    bool read(cv::Mat& image) final;

    friend void grabbingCallback(const CFrame& pFrame, const void* pUser);

private:
    ICameraPtr cameraSptr;
    IStreamSourcePtr streamPtr;

    CircularQueue<cv::Mat, 3> imageQueue;

    int width;
    int height;
    bool exposure_auto;
    double exposure_time;
    double frame_rate;
    int brightness;
    double gamma;
};

void grabbingCallback(const CFrame& pFrame, const void* pUser);
