#ifndef IPOINT_GENERATOR_H
#define IPOINT_GENERATOR_H

#include "Geometry.h"

class IPointGenerator {
public:
    virtual ~IPointGenerator() {}
    virtual std::vector<Point> generatePoints() const = 0;
};

#endif
