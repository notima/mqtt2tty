#ifndef CL_PTY
#define CL_PTY

#include "tty.h"

/**
 * Pseudo terminal extention of Tty class
 */
class Pty : public Tty {
    protected:
        char slaveName[256];
        const char* symlinkName;
        int slaveFileDescriptor;
        void createSymlink();

    public:

        /**
         * @tparam name The full path to the symlink file that the tty
         *      can be accessed through.
         */
        Pty(const char* name);

        /**
         * Write to the tty. 
         * (method name "write" would conflict with method in unistd.h)
         * 
         * @tparam data The text to write.
         */
        void type(const char* data);

        void openTty();
};

#endif