
#include "SerialDevice.h"

SerialDevice::SerialDevice(LPCSTR port_name)
    : hComm(INVALID_HANDLE_VALUE)
    , port_name_(port_name)
    , baudrate_(BAUD_115200)
    , byte_size_(8)
    , parity_bits_('N')
    , stop_bits_(1) {};

SerialDevice::~SerialDevice()
{
    CloseDevice();
}

bool SerialDevice::Init()
{
    return false;
}

int SerialDevice::Read(uint8_t* buf, int len)
{
    return 0;
}

int SerialDevice::Write(const uint8_t* buf, int len)
{
    return 0;
}

bool SerialDevice::OpenDevice()
{
    hComm = CreateFileA(
        port_name_,                     // 串口名，如 COM1, COM2 等；串口号大于9的串口（如 COM12）串口名应写作"\.\COM12"
        GENERIC_READ | GENERIC_WRITE,   // 串口读写权限，允许读写
        0,                              // 共享模式，0表示不共享
        NULL,                           // 安全属性，默认
        OPEN_EXISTING,                  // 打开方式，设备必须存在才打开
        0,
        0);
    return hComm != INVALID_HANDLE_VALUE;
}

bool SerialDevice::CloseDevice()
{
    return false;
}

bool SerialDevice::ConfigDevice()
{
    DCB dcb;
    dcb.BaudRate = baudrate_;
    dcb.StopBits = stop_bits_;
    dcb.ByteSize = byte_size_;
    switch (parity_bits_) {
    case 'N':
        dcb.fParity = FALSE;
        break;
    case 'O':
        dcb.fParity = TRUE;
        dcb.Parity = ODDPARITY;
        break;
    case 'E':
        dcb.fParity = TRUE;
        dcb.Parity = EVENPARITY;
        break;
    default:
        dcb.fParity = FALSE;
        break;
    }
    
    return false;
}
