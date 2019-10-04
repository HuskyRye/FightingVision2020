#include "FightingSimpleCapture.h"

FightingSimpleCapture::FightingSimpleCapture(int camera_id)
{
    capture.open(camera_id);
    capture.set(cv::CAP_PROP_EXPOSURE, -7);
}

bool FightingSimpleCapture::init()
{
    return capture.isOpened();
}

bool FightingSimpleCapture::read(cv::Mat& image)
{
    return capture.read(image);
}
