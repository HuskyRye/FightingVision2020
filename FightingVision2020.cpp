/*
 *  Copyright (C) 2019 刘臣轩
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FightingVision2020.h"

int main(int argc, char* argv[])
{
    /* Video source */
    cameraParam.LoadParam();
    FightingCapture* capture;
    if (cameraParam.camera_type == "MindVision")
        capture = new FightingMVCapture();
    else if (cameraParam.camera_type == "USB")
        capture = new FightingUSBCapture(stoi(cameraParam.camera_name));
    else if (cameraParam.camera_type == "Video")
        capture = new FightingVideoCapture(cameraParam.camera_name);
    else {
        printf("Invalid Camera Type: %s.\n", cameraParam.camera_type.c_str());
        return 1;
    }
    printf("Camera name: %s\n", cameraParam.camera_name.c_str());
    if (capture && !capture->init()) {
        printf("Video source initialization failed.\n");
        return 1;
    }

    /* SerialPort */
#ifdef Windows
    SerialPort serial_port("COM1");
#elif defined Linux
    SerialPort serial_port("/dev/ttyS0");
#endif
    if (!serial_port.Init()) {
        printf("Serial_port initialization failed.\n");
        return 1;
    }
    Protocol protocol(serial_port);
    // std::thread receive(&Protocol::receiveData, protocol);
    // protocol.receiveData();

    /* ArmorDetector and RuneDetector */
    armorParam.LoadParam();
    runeParam.LoadParam();
    ArmorDetector armor_detector;
    RuneDetector rune_detector;
    cv::Mat src;
    cv::Point3f target;

    /* Video writer */
    time_t currentTime = std::time(NULL);
    char chCurrentTime[64];
    std::strftime(chCurrentTime, sizeof(chCurrentTime), "%Y%m%d%H%M", std::localtime(&currentTime));
    std::string stCurrentTime = chCurrentTime;
    // cv::VideoWriter video_writer(stCurrentTime + ".avi", 0, 210, cv::Size(1280, 1024), true);

    while (true) {
        if (capture->read(src)) {
            if ((mcu_data.state == State::ARMOR_STATE && armor_detector.DetectArmor(src, target))
                || (mcu_data.state == State::RUNE_STATE && rune_detector.DetectRune(src, target)))
                protocol.sendTarget(target);
            // cv::imshow("src", src);
            // cv::waitKey(0);
            // video_writer.write(src);
        } else {
            protocol.sendTarget(cv::Point3f(0, 0, -1));
            break;
        }
    }
    return 0;
}
