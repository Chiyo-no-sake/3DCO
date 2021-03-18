//
// Created by kalu on 3/17/21.
//
#include "file_utils.h"

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