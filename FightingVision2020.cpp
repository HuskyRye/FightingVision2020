
#include "FightingVision2020.h"

using namespace std;

int main(int argc, char* argv[])
{
    /* Video source */
    FightingCapture* capture;
    capture = new FightingVideoCapture("D:\\大学\\RoboMaster\\FightingVision2020\\wind.mp4");
    // capture = new FightingCameraCapture();
    // capture = new FightingSimpleCapture(1);
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
    /*
    if (!serial_port.Init()) {
        printf("Serial_port initialization failed.\n");
        return 1;
    }
    */
    Protocol protocol(serial_port);
    

    State current_state = State::RUNE_STATE;
    State last_state = State::ARMOR_STATE;
    ArmorDetector armor_detector;
    RuneDetector rune_detector;

    cv::Mat src;
    cv::Point3f target_3d;
    // TODO: 如果自瞄和大幅参数一致，就把switch_case优化掉
    while (true) {
        switch (current_state) {
        case State::ARMOR_STATE:
            /* 对相机参数进行修改或重启相机
            if (last_state == State::ENERGY_STATE) {
            }*/
            if (capture->read(src)) {
                if (armor_detector.DetectArmor(src, target_3d))
                    protocol.sendTarget(target_3d);
                cv::imshow("armor src", src);
                cv::waitKey(1);
            }
            break;
        case State::RUNE_STATE:
            /* 对相机参数进行修改或重启相机
            if (last_state == State::ARMOR_STATE) {    
            }*/
            if (capture->read(src)) {
                if (rune_detector.DetectRune(src, target_3d))
                    protocol.sendTarget(target_3d);
                cv::imshow("rune src", src);
                cv::waitKey(1);
            }
            break;
        }
        last_state = current_state;
    }
    return 0;
}
