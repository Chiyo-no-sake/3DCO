//
// Created by Simone Belingheri on 10/03/2021.
//

#include <log/Log.h>
#include "OptimizationPipeline.h"

OptimizationPipeline::OptimizationPipeline(coScene *mScene) : m_scene(mScene) {}

void OptimizationPipeline::append(OptimizationStep *step) {
    step->injectScene(m_scene);
    m_optimizations.push_back(step);
}

coScene *OptimizationPipeline::getResult() const {
    return m_scene;
}

void OptimizationPipeline::execute() {

    CO_LOG_INFO("Initiating optimization pipeline...");

    for(auto &step : m_optimizations)
        step->execute();

    CO_LOG_INFO("Optimization pipeline completed");

}
