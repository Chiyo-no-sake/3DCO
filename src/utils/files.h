#pragma once

#ifdef _WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#define SEPARATOR "\\"
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#define SEPARATOR "/"
#endif
