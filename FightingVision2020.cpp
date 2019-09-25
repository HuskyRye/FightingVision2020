
#include "FightingVision2020.h"

using namespace std;

int main()
{
    // 相机初始化
    FightingCapture* capture;
    // capture = new FightingVideoCapture("D:\\VLOG\\中秋\\C0007.MP4");
    capture = new FightingCameraCapture();
    if (!capture->init())
        printf("Video_source initialization failed.");

    // 串口接收线程

    // 状态机
    bool ok = true;
    State current_state = State::ARMOR_STATE;
    State last_state = State::ARMOR_STATE;

    cv::Mat src;
    while (ok) {
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
                cv::imshow("armor src", src);
                cv::waitKey(1);
            }
        }
        last_state = current_state; // 更新状态
    }

    delete capture;
    return 0;
}
