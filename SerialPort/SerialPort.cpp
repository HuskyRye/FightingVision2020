#include "SerialPort.h"

#ifdef Windows

SerialPort::SerialPort(const char* port_name)
    : hComm(INVALID_HANDLE_VALUE)
    , port_name_(port_name)
    , baudrate_(115200)
    , data_bits_(8)
    , parity_bits_(NOPARITY)
    , stop_bits_(ONESTOPBIT)
{
}

SerialPort::~SerialPort()
{
    CloseDevice();
    hComm = INVALID_HANDLE_VALUE;
}

bool SerialPort::Init()
{
    if (nullptr == port_name_)
        port_name_ = "COM1";
    if (OpenDevice() && ConfigDevice()) {
        return true;
    } else {
        printf("Failed to start serial prot.\n");
        CloseDevice();
        return false;
    }
}

int SerialPort::Read(uint8_t* buf, int len)
{
    if (NULL == buf)
        return -1;
    DWORD BytesRead = 0;
    BOOL bResult = false;

    bResult = ReadFile(hComm, buf, len, &BytesRead, NULL);
    if (!bResult) {
        DWORD dwError = GetLastError();
        printf("Connection closed, try to reconnect.\n");
        while (!Init())
            Sleep(500);
        printf("Reconnect Success.\n");
        bResult = ReadFile(hComm, buf, len, &BytesRead, NULL);
    }
    return bResult;
}

int SerialPort::Write(const uint8_t* buf, int len)
{
    // BOOL bResult = false;
    DWORD BytesToWrite = 0;
    WriteFile(hComm, buf, len, &BytesToWrite, NULL);
    /*
    if (!bResult) {
        DWORD dwError = GetLastError();
        return false;
    }*/
    return BytesToWrite;
}

bool SerialPort::OpenDevice()
{
    /* 打开串口 */
    hComm = CreateFileA(port_name_, // 串口名称
        GENERIC_READ | GENERIC_WRITE, // 允许读写
        0, // 独占访问
        NULL, // 默认安全
        OPEN_EXISTING, // 仅打开存在
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

#elif defined Linux

SerialPort::SerialPort(const char* port_name)
    : serial_fd_(-1)
    , port_name_(port_name)
    , baudrate_(B115200)
    , data_bits_(8)
    , parity_bits_('N')
    , stop_bits_(1)
{
}

SerialPort::~SerialPort()
{
    CloseDevice();
}

bool SerialPort::Init()
{
    if (nullptr == port_name_)
        port_name_ = "/dev/ttyUSB0";
    if (OpenDevice() && ConfigDevice()) {
        fd_set serial_fd_set_;
        FD_ZERO(&serial_fd_set_);
        FD_SET(serial_fd_, &serial_fd_set_);
        return true;
    } else {
        printf("Failed to start serial prot.\n");
        CloseDevice();
        return false;
    }
}

bool SerialPort::OpenDevice()
{
#ifdef __arm__
    serial_fd_ = open(port_name_, O_RDWR | O_NONBLOCK);
#else
    serial_fd_ = open(port_name_, O_RDWR | O_NOCTTY);
#endif
    if (serial_fd_ < 0) {
        return false;
        return true;
    }
}

bool SerialPort::CloseDevice()
{
    close(serial_fd_);
    serial_fd_ = -1;
    return true;
}

bool SerialPort::ConfigDevice()
{
    struct termios new_termios_;
    memset(&new_termios_, 0, sizeof(new_termios_));

    /* config the size of char */
    new_termios_.c_cflag |= CLOCAL | CREAD;
    new_termios_.c_cflag &= ~CSIZE;

    /* config data bit */
    switch (data_bits_) {
    case 7:
        new_termios_.c_cflag |= CS7;
        break;
    case 8:
        new_termios_.c_cflag |= CS8;
        break;
    default:
        new_termios_.c_cflag |= CS8;
        break; //8N1 default config
    }

    /* config the parity bit */
    switch (parity_bits_) {
    /* odd */
    case 'O':
    case 'o':
        new_termios_.c_cflag |= PARENB;
        new_termios_.c_cflag |= PARODD;
        break;
    /* even */
    case 'E':
    case 'e':
        new_termios_.c_cflag |= PARENB;
        new_termios_.c_cflag &= ~PARODD;
        break;
    /* none */
    case 'N':
    case 'n':
        new_termios_.c_cflag &= ~PARENB;
        break;
    default:
        new_termios_.c_cflag &= ~PARENB;
        break; //8N1 default config
    }

    /* config baudrate */
    cfsetispeed(&new_termios_, baudrate_);
    cfsetospeed(&new_termios_, baudrate_);

    /* config stop bit */
    if (stop_bits_ == 1)
        new_termios_.c_cflag &= ~CSTOPB;
    else if (stop_bits_ == 2)
        new_termios_.c_cflag |= CSTOPB;
    else
        new_termios_.c_cflag &= ~CSTOPB; //8N1 default config

    /* config waiting time & min number of char */
    new_termios_.c_cc[VTIME] = 1;
    new_termios_.c_cc[VMIN] = 18;

    /* using the raw data mode */
    new_termios_.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    new_termios_.c_oflag &= ~OPOST;

    /* flush the hardware fifo */
    tcflush(serial_fd_, TCIFLUSH);

    /* activite the configuration */
    if ((tcsetattr(serial_fd_, TCSANOW, &new_termios_)) != 0)
        return false;
    return true;
}

int SerialPort::Read(uint8_t* buf, int len)
{
    int ret = -1;
    if (NULL == buf) {
        return -1;
    } else {
        ret = read(serial_fd_, buf, len);
        while (ret == 0) {
            printf("Connection closed, try to reconnect.\n");
            while (!Init()) {
                usleep(500000);
            }
            printf("Reconnect Success.\n");
            ret = read(serial_fd_, buf, len);
        }
        return ret;
    }
}

int SerialPort::Write(const uint8_t* buf, int len)
{
    return write(serial_fd_, buf, len);
}

#endif // OS
