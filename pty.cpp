#include "pty.h"
#include "log.h"

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

/**
 * List of all symlinks that have been created by the program.
 * These symlinks will be deleted when the program is exited.
 */
static std::vector<std::string> createdSymlinks;

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
		LOG_INFO("Removing symlink %s", createdSymlinks.back().c_str());
		if(remove(createdSymlinks.back().c_str())) {
			perror("Symlink can not be destroyed");
		}else{
		}
		createdSymlinks.pop_back();
	}
}

Pty::Pty (const char* name) : Tty(name) {
    symlinkName = name;
    openTty();
    createSymlink();
}

void Pty::type(const char* data) {
	std::string dataStr(data);
	size_t dataLength = dataStr.length();
	(void) write(masterFileDescriptor, data, dataLength);
}

void Pty::openTty(){

    struct termios tt;

    if (tcgetattr (STDIN_FILENO, &tt) < 0)
    {
        LOG_ERROR("Cannot get terminal attributes of stdin");
        exit(1);
    }
    cfmakeraw (&tt);
    if (openpty (&masterFileDescriptor, &slaveFileDescriptor, slaveName, &tt, NULL) < 0)
    {
        LOG_ERROR("Cannot open pty");
        exit(1);
    }
}

void Pty::createSymlink(){
    if(symlink(slaveName, symlinkName)){
        LOG_ERROR("Could not create symlink (%s)", symlinkName);
        //exit(1);
    }else{
		LOG_INFO("Created symlink %s 🔗 %s", symlinkName, slaveName);
		chmod(symlinkName, 444);

		createdSymlinks.push_back(symlinkName);
		if(!unlinkRegistered) {
			std::atexit(unlink);
			unlinkRegistered = true;
		}
	}
}