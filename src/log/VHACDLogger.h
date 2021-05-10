#pragma once

#include <VHACD.h>
#include <string>
#include <fstream>

class VHACDLogger : public VHACD::IVHACD::IUserLogger
{
public:
    explicit VHACDLogger(const std::string & fileName){ OpenFile(fileName);  }

    void Log(const char * const msg) override
    {
        if (m_file.is_open())
        {
            m_file << msg;
            m_file.flush();
        }
    };
    void OpenFile(const std::string & fileName) { m_file.open(fileName.c_str()); }
private:
    std::ofstream m_file;
};

