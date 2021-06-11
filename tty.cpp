#include "tty.h"

#include <sys/stat.h>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <pty.h>

#undef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#define concat(first, second) first second

/**
 * List of all symlinks that have been created by the program.
 * These symlinks will be deleted when the program is exited.
 */
std::vector<const char*> createdSymlinks;

/**
 * Has the unlink method already been registered at exit?
 * This is used to not register the method multiple times if
 * there are multiple instances of Tty.
 */
bool unlinkRegistered;

/**
 * Delete all created symlinks. This will run when the program
 * is exited.
 */
void unlink(){
	while(createdSymlinks.size() > 0){
		if(remove(createdSymlinks.back())) {
			perror("Symlink can not be destroyed");
		}else{
			std::string msg("Removing symlink ");
			msg.append(createdSymlinks.back());
			puts(msg.c_str());
		}
		createdSymlinks.pop_back();
	}
}

Tty::Tty (const char* name)
{
    symlinkName = name;
    openTty();
    createSymlink();
}

void Tty::type(char* data) {
	std::string dataStr(data);
	size_t dataLength = dataStr.length();
	(void) write(masterFileDescriptor, data, dataLength);
}

void Tty::openTty(){

    struct termios tt;

    if (tcgetattr (STDIN_FILENO, &tt) < 0)
    {
        perror("Cannot get terminal attributes of stdin");
        exit(1);
    }
    cfmakeraw (&tt);
    if (openpty (&masterFileDescriptor, &slaveFileDescriptor, slaveName, &tt, NULL) < 0)
    {
        perror("Cannot open pty");
        exit(1);
    }
}

void Tty::createSymlink(){
    if(symlink(slaveName, symlinkName)){
        perror("Could not create symlink");
        //exit(1);
    }else{
		chmod(symlinkName, 444);

		createdSymlinks.push_back(symlinkName);
		if(!unlinkRegistered) {
			std::atexit(unlink);
			unlinkRegistered = true;
		}
	}
}