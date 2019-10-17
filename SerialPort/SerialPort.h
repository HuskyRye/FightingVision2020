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

#pragma once

#include <iostream>
#include <string>

#ifdef Windows
#include <Windows.h>
#elif defined Linux
#include <cstring>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#endif

class SerialPort {
public:
    SerialPort(const char* port_name);
    ~SerialPort();

    bool Init();
    int Read(uint8_t* buf, int len);
    int Write(const uint8_t* buf, int len);

private:
    bool OpenDevice();
    bool CloseDevice();
    bool ConfigDevice();

#ifdef Windows
    HANDLE hComm;
#elif defined Linux
    int serial_fd_;
#endif

    const char* port_name_;
    unsigned long baudrate_;
    unsigned char stop_bits_;
    unsigned char data_bits_;
    unsigned char parity_bits_;
};


