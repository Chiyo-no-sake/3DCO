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

//std::string getWinTempDirectory(){
//    char *buffer = (char*)malloc(MAX_PATH);
//    GetTempPathA(MAX_PATH, buffer);
//    std::string ret{buffer};
//    free(buffer);
//    return ret;
//}

int executeCommand(const std::string& cmd, bool verbose);