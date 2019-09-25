#pragma once

#include <string>
#define Windows
#ifdef Windows
#include <Windows.h>
class SerialDevice {
public:
    SerialDevice(LPCSTR port_name);
    ~SerialDevice();

    bool Init();
    int Read(uint8_t* buf, int len);
    int Write(const uint8_t* buf, int len);

private:
    bool OpenDevice();
    bool CloseDevice();
    bool ConfigDevice();

    HANDLE hComm;
    LPCSTR port_name_;
    DWORD baudrate_;
    BYTE stop_bits_;
    BYTE byte_size_;
    BYTE parity_bits_;

    //! termios config for serial handler
    // struct termios new_termios_, old_termios_;
};

#elif defined Linux
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#endif // Windows or Linux

