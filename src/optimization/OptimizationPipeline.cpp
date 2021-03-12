//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "OptimizationPipeline.h"

OptimizationPipeline::OptimizationPipeline() {}

OptimizationPipeline::OptimizationPipeline(coScene *mScene) : m_scene(mScene) {}

void OptimizationPipeline::setScene(coScene *mScene) {
    m_scene = mScene;
}

void OptimizationPipeline::append(OptimizationStep *step) {
    m_optimizations.push_back(step);
}

coScene *OptimizationPipeline::getResult() const {
    return m_scene;
}

void OptimizationPipeline::execute() {

    CO_LOG_INFO("Initiating optimization pipeline...");

    for(coMesh *mesh : m_scene->getMMeshes())
        for(auto &step : m_optimizations){
            step->injectMesh(mesh);
            step->execute();
    }

    CO_LOG_INFO("Optimization pipeline completed");

}
