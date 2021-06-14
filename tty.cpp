#include "tty.h"
#include "log.h"
#include <string.h>
#include <iostream>
#include <fstream>

#undef max
#define max(x,y) ((x) > (y) ? (x) : (y))

Tty::Tty (const char* name)
{
	fileName = name;
}

void Tty::type(const char* data) {
	ofstream ttyFile;
	ttyFile.open (fileName.c_str());
	ttyFile << data;
	ttyFile.close();
}