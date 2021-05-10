#pragma once

#include <string>
#include <stdio.h>
// getting current directory
#ifdef _WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#define SEPARATOR "\\"
#define EXE_POSTFIX ".exe"
#define P_OPEN _popen
#define P_CLOSE _pclose
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#define SEPARATOR "/"
#define EXE_POSTFIX ""
#define P_OPEN popen
#define P_CLOSE pclose
#endif

int executeCommand(const std::string& cmd, bool verbose);

std::string getDirectoryFor(const std::string& path);