//
// Created by Simone Belingheri on 19/03/2021.
//

#ifndef INC_3DCO_VHACDLOGCALLBACK_H
#define INC_3DCO_VHACDLOGCALLBACK_H


#include <VHACD.h>
#include "Log.h"

class VHACDLogCallback : public VHACD::IVHACD::IUserCallback
{
public:


    void Update(const double          overallProgress,
                const double          stageProgress,
                const double          operationProgress,
                const char * const    stage,
                const char * const    operation) override
    {
        CO_LOG_TRACE("VHACD [\r\n \toverall progress: {} "
                     "\r\n \tstage: {} "
                     "\r\n \tstage progress: {} "
                     "\r\n \toperation: {}"
                     "\r\n \toperation progress: {} "
                     "\r\n]", overallProgress, stage, stageProgress, operation, operationProgress);
    };
};


#endif //INC_3DCO_VHACDLOGCALLBACK_H
