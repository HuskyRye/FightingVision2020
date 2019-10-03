
#include "FightingVision2020.h"

using namespace std;

void uartReceive(SerialPort* uart)
{
    uint8_t buf[4096];
    while (true) {
        memset(buf, 0, sizeof(buf));
        uart->Read(buf, 4096);
        printf("%s", buf);
    }
}

int main()
{
    // 相机初始化
    FightingCapture* capture;
    // capture = new FightingVideoCapture("D:\\VLOG\\中秋\\C0007.MP4");
    capture = new FightingCameraCapture();
    if (!capture->init())
        printf("Video_source initialization failed.\n");

    // 串口接收线程
    SerialPort device("COM1");
    if (!device.Init())
        printf("Serial device initialization failed.\n");

    /* 串口测试 
    thread receive(uartReceive, &device);
    const uint8_t buf[] = "Hello world!\n";
    while (true) {
        device.Write(buf, sizeof(buf));
        Sleep(1000);
    }*/

    // 状态机
    State current_state = State::ARMOR_STATE;
    State last_state = State::ARMOR_STATE;

    ArmorDetector detector;

    cv::Mat src;
    while (true) {
        if (current_state == State::ENERGY_STATE) { // 能量机关状态
            if (last_state == State::ARMOR_STATE) {
                //  对相机参数进行修改或重启相机
            }
            if (capture->read(src)) {
                // 能量机关
                cv::imshow("energy src", src);
                cv::waitKey(1);
            }
        } else { // 自瞄状态
            if (last_state == State::ENERGY_STATE) {
                // 对相机参数进行修改或重启相机
            }
            if (capture->read(src)) {
                // 自瞄
                cv::Point3f target_3d;
                detector.DetectArmor(src, target_3d);
                cv::imshow("armor src", src);
                cv::waitKey(1);
            }
        }
        last_state = current_state; // 更新状态
    }

    delete capture;
    return 0;
}
