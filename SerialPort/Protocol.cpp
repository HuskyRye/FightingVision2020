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

void Protocol::sendTarget(cv::Point3f& target)
{
    Target t { target.x, target.y, target.z };
    unsigned char uart_data[16];
    uart_data[0] = 0x03;
    uart_data[1] = 0xFC;
    memcpy(uart_data + 2 * sizeof(uchar), &t, sizeof(t));
    uart_data[14] = 0xFC;
    uart_data[15] = 0x03;
    serial_port_.Write(uart_data, sizeof(uart_data));
}

McuData mcu_data = {
    EnemyColor::BLUE,
    State::ARMOR_STATE
};

void Protocol::receiveData()
{
    uint8_t buffer[6];
    bool init = false;
    while (!init) {
        memset(buffer, 0, sizeof(buffer));
        serial_port_.Read(buffer, sizeof(mcu_data) + 4);
        if (buffer[0] == 0x03 && buffer[1] == 0xFC
            && buffer[sizeof(mcu_data) + 2] == 0xFC
            && buffer[sizeof(mcu_data) + 3] == 0x03) {
            memcpy(&mcu_data, buffer + 2, sizeof(mcu_data));
            init = true;
        }
    }
}
