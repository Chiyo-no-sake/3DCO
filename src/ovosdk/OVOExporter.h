#pragma once

#include "model/coScene.h"

class OVOExporter {
public:
    static bool exportTo(coScene* scene, char* path);
};
