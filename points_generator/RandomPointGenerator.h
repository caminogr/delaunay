#ifndef RANDOM_POINT_GENERATOR_H
#define RANDOM_POINT_GENERATOR_H

#include "Geometry.h"
#include "IPointGenerator.h"

class RandomPointGenerator : public IPointGenerator {
public:
  std::vector<Point> generatePoints() const override;
};

#endif
