#include "PointsGenerator.h"

PointsGenerator::PointsGenerator(std::shared_ptr<IPointGenerator> generator) : pointGenerator(std::move(generator)) {}

std::vector<Point> PointsGenerator::generate() {
    return pointGenerator->generatePoints();
}
