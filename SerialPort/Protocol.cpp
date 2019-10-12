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
