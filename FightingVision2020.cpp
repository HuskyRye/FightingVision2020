
#include "FightingVision2020.h"

using namespace std;

int main(int argc, char* argv[])
{
    // 相机初始化
    FightingCapture* capture;
    // capture = new FightingVideoCapture("D:\\VLOG\\中秋\\C0007.MP4");
    capture = new FightingCameraCapture();
    // capture = new FightingSimpleCapture(1);
    if (!capture->init())
        printf("Video_source initialization failed.\n");

    // 串口初始化
    SerialPort serial_port("COM5");
    if (!serial_port.Init())
        printf("Serial_port initialization failed.\n");
    Protocol protocol(serial_port);

    // 状态机
    State current_state = State::ARMOR_STATE;
    State last_state = State::ARMOR_STATE;
    ArmorDetector detector;

    cv::Mat src;
    while (true) {
        switch (current_state) {
        case State::ENERGY_STATE:
            if (last_state == State::ARMOR_STATE) {
                //  对相机参数进行修改或重启相机
            }
            if (capture->read(src)) {
                // 能量机关
                cv::imshow("energy src", src);
                cv::waitKey(1);
            }
            break;
        case State::ARMOR_STATE:
            if (last_state == State::ENERGY_STATE) {
                // 对相机参数进行修改或重启相机
            }
            if (capture->read(src)) {
                cv::Point3f target_3d;
                if (detector.DetectArmor(src, target_3d))
                    protocol.sendTarget(target_3d); // 发送数据
                cv::imshow("armor src", src);
                cv::waitKey(1);
            }
            break;
        }
        last_state = current_state; // 更新状态
    }
    return 0;
}
