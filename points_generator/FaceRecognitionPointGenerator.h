#ifndef FACE_RECOGNITION_POINT_GENERATOR_H
#define FACE_RECOGNITION_POINT_GENERATOR_H

#include "Geometry.h"
#include "IPointGenerator.h"

class FaceRecognitionPointGenerator : public IPointGenerator {
public:
  std::vector<Point> generatePoints() const override;
};

#endif
