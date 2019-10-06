#pragma once

#include "SerialPort.h"
#include "opencv2/opencv.hpp"

class Protocol {
public:
    explicit Protocol(SerialPort& serial_port);
    ~Protocol();
    void sendTarget(cv::Point3f& target_3d);
private:
    SerialPort& serial_port_;
};