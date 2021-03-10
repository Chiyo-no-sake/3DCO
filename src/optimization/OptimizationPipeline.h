//
// Created by Simone Belingheri on 10/03/2021.
//

#ifndef INC_3DCO_OPTIMIZATIONPIPELINE_H
#define INC_3DCO_OPTIMIZATIONPIPELINE_H

#include <common/IExecutable.h>
#include <vector>
#include <model/coScene.h>
#include "OptimizationStep.h"

class OptimizationPipeline : public IExecutable{

private:

    std::vector<OptimizationStep*> m_optimizations;
    coScene *m_scene;

public:

    explicit OptimizationPipeline(coScene *mScene);

    void append(OptimizationStep *step);

    [[nodiscard]] coScene *getResult() const;

    void execute() override;

};


#endif //INC_3DCO_OPTIMIZATIONPIPELINE_H
