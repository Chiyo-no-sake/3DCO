//
// Created by Simone Belingheri on 19/03/2021.
//

#include "VHACDGenerator.h"

VHACDGenerator *VHACDGenerator::getInstance() {

    static VHACDGenerator instance;

    return &instance;

}

void VHACDGenerator::init() {

    m_interface = VHACD::CreateVHACD();

}

void VHACDGenerator::compute(coScene *scene) {

    

}

void VHACDGenerator::deinit() {

    m_interface->Clean();
    m_interface->Release();

}
