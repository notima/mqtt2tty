#ifndef CL_TTY
#define CL_TTY

#include <termios.h>

class Tty {
    protected:
        char slaveName[256];
        const char* symlinkName;
        struct termios tt;
        int masterFileDescriptor;
        int slaveFileDescriptor;

        void openTty();
        void createSymlink();

    public:

        /**
         * @tparam name The full path to the symlink file that the tty
         *      can be accessed through.
         */
        Tty(const char* name);

        /**
         * Write to the tty. 
         * (method name "write" would conflict with method in unistd.h)
         * 
         * @tparam data The text to write.
         */
        void type(char* data);
};

#endif