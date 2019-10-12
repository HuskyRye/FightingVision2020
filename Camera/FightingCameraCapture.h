
#pragma once

#include "FightingCapture.h"

#include "GenICam/Camera.h"
#include "GenICam/StreamSource.h"
#include "GenICam/System.h"
#include "ImageConvert.h"

#include "CircularQueue.h"
#include "FightingCameraParam.h"

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
};

void grabbingCallback(const CFrame& pFrame, const void* pUser);
