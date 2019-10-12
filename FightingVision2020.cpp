
#include "FightingVision2020.h"

enum class State { ARMOR_STATE,
    RUNE_STATE };

int main(int argc, char* argv[])
{
    /* Video source */
    FightingCapture* capture;
    capture = new FightingVideoCapture("D:\\大学\\RoboMaster\\RM2019能量机关视频\\Big_Red_Light.MOV");
    // capture = new FightingCameraCapture();
    // capture = new FightingSimpleCapture(1);
    LoadCameraParam();
    if (!capture->init()) {
        printf("Video source initialization failed.\n");
        return 1;
    }

    /* SerialPort */
#ifdef Windows
    SerialPort serial_port("COM1");
#elif defined Linux
    SerialPort serial_port("/dev/ttyUSB0");
#endif
    if (!serial_port.Init()) {
        printf("Serial_port initialization failed.\n");
        return 1;
    }
    Protocol protocol(serial_port);
   
    State current_state = State::RUNE_STATE;
    ArmorDetector armor_detector;
    RuneDetector rune_detector;
    cv::Mat src;
    cv::Point3f target;
    while (true) {
        if (capture->read(src)) {
            if ((current_state == State::ARMOR_STATE && armor_detector.DetectArmor(src, target)) 
                || (current_state == State::RUNE_STATE && rune_detector.DetectRune(src, target)))
                protocol.sendTarget(target);
            // cv::imshow("src", src);
            cv::waitKey(0);
        }
    }
    return 0;
}
