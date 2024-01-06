#ifndef POINTS_GENERATOR_H
#define POINTS_GENERATOR_H

#include <memory>
#include <vector>
#include "IPointGenerator.h"

class PointsGenerator {
public:
    explicit PointsGenerator(std::shared_ptr<IPointGenerator> generator);
    std::vector<Point> generate();

private:
    std::shared_ptr<IPointGenerator> pointGenerator;
};

#endif
