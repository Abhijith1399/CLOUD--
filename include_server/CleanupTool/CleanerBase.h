#pragma once
#ifndef CLEANERBASE_H
#define CLEANERBASE_H

#include <string>

class CleanerBase {
public:
    virtual ~CleanerBase() = default;
    virtual void clean() = 0; // Pure virtual function for cleanup action
};

#endif // CLEANERBASE_H
