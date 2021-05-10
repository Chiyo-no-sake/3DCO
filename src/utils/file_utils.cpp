//
// Created by kalu on 3/17/21.
//
#include "file_utils.h"

std::string getDirectoryFor(const std::string &path) {
    int cutIndex = path.find_last_of(SEPARATOR);

    if (cutIndex == std::string::npos) {
        return "";
    }

    return path.substr(0, cutIndex + 1);
}

int executeCommand(const std::string &cmd, bool verbose) {
    std::string result;

    auto pipe = P_OPEN((std::string(".") + SEPARATOR + cmd).c_str(), "r"); // get rid of shared_ptr

    if (!pipe) return 1;

    if (verbose)
        while (!feof(pipe)) {
            char c = (char) fgetc(pipe);
            if (c != EOF)
                putchar(c);
        }

    return P_CLOSE(pipe);
}