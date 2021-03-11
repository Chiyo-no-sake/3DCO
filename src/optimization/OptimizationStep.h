//
// Created by Simone Belingheri on 10/03/2021.
//

#ifndef INC_3DCO_OPTIMIZATIONSTEP_H
#define INC_3DCO_OPTIMIZATIONSTEP_H


#include <common/IExecutable.h>
#include <model/coScene.h>

class OptimizationStep : public IExecutable{

protected:

    coMesh *m_mesh;

public:

    void injectMesh(coMesh *mesh);

};


#endif //INC_3DCO_OPTIMIZATIONSTEP_H
