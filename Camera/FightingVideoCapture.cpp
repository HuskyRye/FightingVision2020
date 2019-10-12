
#include "FightingVideoCapture.h"

FightingVideoCapture::FightingVideoCapture(const std::string& filename)
{
    capture.open(filename);
}

bool FightingVideoCapture::init()
{
    return capture.isOpened();
}

bool FightingVideoCapture::read(cv::Mat& image)
{
    return capture.read(image);
}
