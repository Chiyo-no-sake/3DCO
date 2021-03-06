#pragma once


class IStrategy {
public:
    virtual void execute() = 0;

    virtual ~IStrategy() {}
};