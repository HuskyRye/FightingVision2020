#pragma once

#include <iostream>
#include <string>

#ifdef Windows
#include <Windows.h>
#elif defined Linux
#include <string.h>
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


