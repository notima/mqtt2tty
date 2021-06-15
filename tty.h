#ifndef CL_TTY
#define CL_TTY

#include <termios.h>
#include <string>

using namespace std;

class Tty {
    protected:
        struct termios tt;
        string fileName;
        int ttyFileDescriptor;

    public:

        /**
         * @tparam name The full path to the tty file.
         */
        Tty(const char* name);

        /**
         * Write to the tty. 
         * (method name "write" would conflict with method in unistd.h)
         * 
         * @tparam data The text to write.
         */
        void type(const char* data);

        virtual void openTty();

        void setBaudRate(int baudRate);
};

#endif