#pragma once

// getting current directory
#ifdef _WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#define SEPARATOR "\\"
#define TempDir getWinTempDirectory()
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#define SEPARATOR "/"
#define TempDir "/tmp/"
#endif

//std::string getWinTempDirectory(){
//    char *buffer = (char*)malloc(MAX_PATH);
//    GetTempPathA(MAX_PATH, buffer);
//    std::string ret{buffer};
//    free(buffer);
//    return ret;
//}