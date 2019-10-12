
#include "Protocol.h"

Protocol::Protocol(SerialPort& serial_port)
    : serial_port_(serial_port)
{

}

Protocol::~Protocol()
{
}

void Protocol::sendTarget(cv::Point3f& target_3d)
{
    union turn {
        float d;
        unsigned char data[4];
    } t;
    unsigned char uart_data[16];
    uart_data[0] = 0x03;
    uart_data[1] = 0xFC;
    t.d = target_3d.x;
    uart_data[2] = t.data[0];
    uart_data[3] = t.data[1];
    uart_data[4] = t.data[2];
    uart_data[5] = t.data[3];
    t.d = target_3d.y;
    uart_data[6] = t.data[0];
    uart_data[7] = t.data[1];
    uart_data[8] = t.data[2];
    uart_data[9] = t.data[3];
    t.d = target_3d.z;
    uart_data[10] = t.data[0];
    uart_data[11] = t.data[1];
    uart_data[12] = t.data[2];
    uart_data[13] = t.data[3];
    uart_data[14] = 0xFC;
    uart_data[15] = 0x03;
    serial_port_.Write(uart_data, 16);
}
