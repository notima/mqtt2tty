#ifndef CL_TTY
#define CL_TTY

#include <termios.h>
#include <string>

using namespace std;

class Tty {
    protected:
        string fileName;

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
};

#endif