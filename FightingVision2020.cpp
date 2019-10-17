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

enum class State { ARMOR_STATE,
    RUNE_STATE };

int main(int argc, char* argv[])
{
    /* Video source */
    cameraParam.LoadParam();
    FightingCapture* capture;
    if (cameraParam.camera_type == "Dahua")
        capture = new FightingDahuaCapture();
    else if (cameraParam.camera_type == "USB")
        capture = new FightingUSBCapture(stoi(cameraParam.camera_name));
    else if (cameraParam.camera_type == "Video")
        capture = new FightingVideoCapture(cameraParam.camera_name);
    else {
        printf("Invalid Camera Type: %s.\n", cameraParam.camera_type);
        return 1;
    }
    if (capture && !capture->init()) {
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

    /* Armor and Rune */
    armorParam.LoadParam();
    runeParam.LoadParam();
    State current_state = State::ARMOR_STATE;
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
            // cv::waitKey(0);
        } else
            break;
    }
    return 0;
}
