#include "SerialPort.h"

SerialPort::SerialPort(LPCTSTR port_name)
    : hComm(INVALID_HANDLE_VALUE)
    , port_name_(port_name)
    , baudrate_(CBR_115200)
    , data_bits_(8)
    , parity_bits_(NOPARITY)
    , stop_bits_(ONESTOPBIT)
{
}

SerialPort::~SerialPort()
{
    CloseDevice();
}

bool SerialPort::Init()
{
    if (port_name_ == nullptr)
        port_name_ = "COM1";
    if (OpenDevice() && ConfigDevice()) {
        return true;
    } else {
        printf("Failed to start serial prot.\n");
        CloseDevice();
        return false;
    }
}

int SerialPort::Read()
{
    return 0;
}

int SerialPort::Write(const uint8_t* buf, int len)
{
    return 0;
}

bool SerialPort::OpenDevice()
{
    /* 打开串口 */
    hComm = CreateFileA(port_name_,     // 串口名称
        GENERIC_READ | GENERIC_WRITE,   // 允许读写
        0,                              // 独占访问
        NULL,                           // 默认安全
        OPEN_EXISTING,                  // 仅打开存在
        0,
        NULL);
    return hComm != INVALID_HANDLE_VALUE;
}

bool SerialPort::CloseDevice()
{
    return CloseHandle(hComm);
}

bool SerialPort::ConfigDevice()
{
    /* 设置不使用超时 
    COMMTIMEOUTS CommTimeouts;
    CommTimeouts.ReadIntervalTimeout = 0;
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.ReadTotalTimeoutConstant = 0;
    CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = 0;
    if (!SetCommTimeouts(hComm, &CommTimeouts)) {
        printf("Set Comm timeouts fail.\n");
        return false;
    } */

    /* 配置串口 */
    DCB dcb;
    GetCommState(hComm, &dcb);
    dcb.BaudRate = baudrate_;
    dcb.StopBits = stop_bits_;
    dcb.ByteSize = data_bits_;
    dcb.Parity = parity_bits_;
    if (!SetCommState(hComm, &dcb)) {
        printf("Set Comm state fail.\n");
        return false;
    }

    /* 清空串口缓冲区 */
    PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    return true;
}