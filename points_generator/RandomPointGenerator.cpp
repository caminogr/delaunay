#include "RandomPointGenerator.h"
#include <cstdlib> 

const int POINTS_NUM = 40;

std::vector<Point> RandomPointGenerator::generatePoints() const {
  std::vector<Point> points;
  for (int i = 0; i < POINTS_NUM; ++i) {
    float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1;
    float y = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2 - 1;
    points.push_back({ x, y });
  }
  return points;
}
