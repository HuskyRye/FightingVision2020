#pragma once

#include <iostream>
#include <string>
#include <Windows.h>

class SerialPort {
public:
    SerialPort(LPCTSTR port_name);
    ~SerialPort();

    bool Init();
    int Read();
    int Write(const uint8_t* buf, int len);

private:
    bool OpenDevice();
    bool CloseDevice();
    bool ConfigDevice();

    HANDLE hComm;
    LPCTSTR port_name_;
    DWORD baudrate_;
    BYTE stop_bits_;
    BYTE data_bits_;
    BYTE parity_bits_;
};