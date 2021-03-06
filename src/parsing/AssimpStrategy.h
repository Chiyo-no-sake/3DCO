#pragma once
#include "ParsingStrategy.h"

class AssimpStrategy : public ParsingStrategy
{
public:
    AssimpStrategy() = default;

    void execute() override;
};

