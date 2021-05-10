#pragma once

#include <VHACD.h>
#include <model/coScene.h>

class VHACDGenerator {
private:

    VHACD::IVHACD::Parameters m_params;
    VHACD::IVHACD *m_interface;

public:

    static VHACDGenerator *getInstance();

    void init();

    void deinit();

    void compute(coScene *scene);

};
