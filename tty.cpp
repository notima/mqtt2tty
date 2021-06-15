#include "tty.h"
#include "log.h"
#include <string.h>
#include <iostream>
#include <fstream>
#include <unistd.h>

#undef max
#define max(x,y) ((x) > (y) ? (x) : (y))

Tty::Tty (const char* name)
{
	fileName = name;
}

void Tty::openTty(){
    ttyFileDescriptor = open(fileName.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if(ttyFileDescriptor == -1) {
		LOG_ERROR("Failed to open %s", fileName.c_str());
	}

    struct termios tt;

    if (tcgetattr (ttyFileDescriptor, &tt) < 0)
    {
        LOG_ERROR("Cannot get terminal attributes of %s", fileName.c_str());
        //exit(1);
    }
    cfmakeraw (&tt);
}

speed_t int2speed(int baudRate) {
	switch(baudRate){
		default: LOG_ERROR ("Unsupported baud rate: %d", baudRate);
		case 0: return B0;
		case 50: return B50;
		case 75: return B75;
		case 110: return B110;
		case 134: return B134;
		case 150: return B150;
		case 200: return B200;
		case 300: return B300;
		case 600: return B600;
		case 1200: return B1200;
		case 1800: return B1800;
		case 2400: return B2400;
		case 4800: return B4800;
		case 9600: return B9600;
		case 19200: return B19200;
		case 38400: return B38400;
		case 57600: return B57600;
		case 115200: return B115200;
		case 230400: return B230400;
		case 460800: return B460800;
		case 500000: return B500000;
		case 576000: return B576000;
		case 921600: return B921600;
		case 1000000: return B1000000;
		case 1152000: return B1152000;
		case 1500000: return B1500000;
		case 2000000: return B2000000;
		case 2500000: return B2500000;
		case 3000000: return B3000000;
		case 3500000: return B3500000;
		case 4000000: return B4000000;
	}
}

void Tty::setBaudRate(int baudRate){

	speed_t speed = int2speed(baudRate);

	struct termios tt;

    if (tcgetattr (ttyFileDescriptor, &tt) < 0)
    {
        LOG_ERROR("Cannot get terminal attributes of %s", fileName.c_str());
    }

	if(cfsetospeed(&tt, speed) == -1 || cfsetispeed(&tt, speed) == -1){
		LOG_ERROR("Failed to set baudrate of %s", fileName.c_str())
	}

	// Set some additional flags
	tt.c_cflag = (tt.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
	// disable IGNBRK for mismatched speed tests; otherwise receive break
	// as \000 chars
	tt.c_iflag &= ~IGNBRK;         // disable break processing
	tt.c_lflag = 0;                // no signaling chars, no echo,
	tt.c_oflag = 0;                // no remapping, no delays
	tt.c_cc[VMIN]  = 0;            // read doesn't block
	tt.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
	tt.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
	tt.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
	tt.c_cflag &= ~(PARENB | PARODD);      // shut off parity
	tt.c_cflag |= 0;
	tt.c_cflag &= ~CSTOPB;
	tt.c_cflag &= ~CRTSCTS;

	if (tcsetattr (ttyFileDescriptor, TCSANOW, &tt) != 0)
	{
		LOG_ERROR("Cannot set terminal attributes of %s", fileName.c_str());
	}
}

void Tty::type(const char* data) {
	LOG_DEBUG("printing message to %s", fileName.c_str());
	std::string dataStr(data);
	size_t dataLength = dataStr.length();
	(void) write(ttyFileDescriptor, data, dataLength);
}