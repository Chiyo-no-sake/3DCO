#pragma once


class IExecutable {
public:
    virtual void execute() = 0;

    virtual ~IExecutable() {}
};